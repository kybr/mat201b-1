#include "common.h"

#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
// struct MyApp :  App {
struct MyApp : OmniStereoGraphicsRenderer {
  vector<Vec3f> pos;
  Data data;
  Texture texture[FILE_LIST_N];

  MyApp() {
    for (int i = 0; i < FILE_LIST_N; i++) {
      Image image;
      if (!image.load(fullPathOrDie(fileList[i], ".."))) {
        cerr << "failed to load " << fileList[i] << endl;
        exit(1);
      }
      texture[i].allocate(image.array());
    }

    //     memset(state, 0, sizeof(state));
    // =======
    memset(state, 0, sizeof(State));
    // >>>>>>> b28d40ce01a258c918356599dd6d36cafc419e6a

    Image background;

    omni().clearColor() = Color(1);

    if (!background.load(fullPathOrDie("possiblebg.png"))) {
      fprintf(stderr, "FAIL\n");
      exit(1);
    }
    backTexture.allocate(background.array());

    addSphereWithTexcoords(backMesh, 1.3);
    backMesh.generateNormals();

    lens().far(1000);

    data.load(fullPathOrDie("finaltennisdata.csv"));

    addSphere(sphere);
    sphere.generateNormals();

    float worldradius = 1;
    cout << data.row[0].monthData.size() << endl;
    for (int i = 0; i < data.row.size(); i++) {
      // cout << i << endl;
      Vec3f position;
      position.x = -worldradius * cos((data.row[i].latitude) * (3.14 / 180)) *
                   cos((data.row[i].longitude) * (3.14 / 180));
      position.y = worldradius * sin((data.row[i].latitude) * (3.14 / 180));
      position.z = worldradius * cos((data.row[i].latitude) * (3.14 / 180)) *
                   sin((data.row[i].longitude) * (3.14 / 180));

      pos.push_back(position);

      for (int j = 0; j < data.row[0].monthData.size(); j++) {
        int outgoing =
            5 + (data.row[i].monthData[j] - 0) * (100 - 5) / (100 - 0);
        int outgoingcolor =
            0 + (data.row[i].colors[j] - 0) * (255 - 0) / (100 - 0);
        data.row[i].monthData[j] = outgoing;

        data.row[i].colors[j] = outgoingcolor;
      }
    }

    initWindow();
  }

  cuttlebone::Taker<State> taker;
  State* state = new State;
  virtual void onAnimate(double dt) {
    if (taker.get(*state) > 0) pose = state->pose;
    if(state->rotation == 1){
      nav().pos(0,0,0);
      nav().spinU(state->angle);
      //state->angle = rateSlider.getValue();
    }
    else{
      nav().spinU(0);
    }
  }

  Material material;
  Light light;
  Mesh sphere;
  // virtual void onDraw(Graphics& g, const Viewpoint& v) {
  virtual void onDraw(Graphics& g) {
    // cout << labels.getValue() << endl;

    if (false) {
      backTexture.bind();
      g.draw(backMesh);
      backTexture.unbind();
    }

    g.depthMask(true);
    g.depthTesting(true);
    g.blending(false);

    material();
    light();

    shader().uniform("texture", 0.0);
    shader().uniform("lighting", 0.5);
    // shader().uniform("COLOR", 0.5);

    for (int i = 0; i < data.row.size(); i++) {
      g.pushMatrix();

      // g.rotate(state->angle, 0, 1, 0);

      for (int j = 0; j < data.row[0].monthData.size(); j++) {
        g.color(HSV(data.row[i].colors[j] / 255.0*2, .65, 1));
      }
      g.translate(pos[i] + pos[i] *
                               data.row[i].monthData[state->indexOfDataSet] *
                               state->course);
      Vec3f src = pos[i] + pos[i] *
                               data.row[i].monthData[state->indexOfDataSet] *
                               state->course;
      double scale = .001;
      g.scale(data.row[i].monthData[state->indexOfDataSet] * scale);
      g.draw(sphere);
      g.popMatrix();
    }

    // g.clear(Graphics::COLOR_BUFFER_BIT);
    g.depthMask(false);
    g.depthTesting(false);
    g.blending(true);
    // g.blendModeAdd();
    g.blendModeTrans();

    shader().uniform("texture", 1.0);
    shader().uniform("lighting", 0.0);

    if (state->turnOnLabels == 1) {
      for (int i = 0; i < data.row.size(); i++) {
        Vec3f src = pos[i] + pos[i] *
                                 data.row[i].monthData[state->indexOfDataSet] *
                                 state->course;
        g.pushMatrix();
        // g.translate(.9, 0, .9);
        g.translate(src);
        Vec3d forward = Vec3d(pose.pos() - src).normalize();
        // Vec3d forward = Vec3d(Vec3f(0, 0, 0) - src).normalize();
        Quatd rot = Quatd::getBillboardRotation(forward, pose.uu());
        g.rotate(rot);
        g.scale(0.07);
        texture[i].quad(g);
        g.popMatrix();
      }
    }
  }

  // virtual void ___onDraw(Graphics& g) {
  //   shader().uniform("lighting", 0.0);
  //   shader().uniform("texture", 1.0);
  //   if (false) {
  //     backTexture.bind();
  //     g.draw(backMesh);
  //     backTexture.unbind();
  //   }

  //   g.depthMask(true);
  //   g.depthTesting(true);
  //   g.blending(false);
  //   material();
  //   light();
  //   shader().uniform("lighting", 0.2);
  //   shader().uniform("texture", 0.0);
  //   for (int i = 0; i < data.row.size(); i++) {
  //     g.pushMatrix();
  //     for (int j = 0; j < data.row[0].monthData.size(); j++) {
  //       g.color(HSV(data.row[i].colors[j] / 255.0, .4, .5));
  //     }
  //     g.translate(pos[i] + pos[i] *
  //                              data.row[i].monthData[state->indexOfDataSet] *
  //                              state->course);
  //     Vec3f src = pos[i] + pos[i] *
  //                              data.row[i].monthData[state->indexOfDataSet] *
  //                              state->course;
  //     double scale = .001;
  //     g.scale(data.row[i].monthData[state->indexOfDataSet] * scale);
  //     g.pushMatrix();
  //     g.translate(.9, 0, .9);
  //     g.popMatrix();
  //     g.draw(sphere);
  //     if (state->turnOnLabels == 1) {
  //       g.pushMatrix();
  //       shader().uniform("lighting", 0.0);
  //       g.translate(.9, 0, .9);
  //       Vec3d forward = Vec3d(Vec3f(0, 0, 0) - src).normalize();
  //       Quatd rot = Quatd::getBillboardRotation(forward, nav().uu());
  //       g.rotate(rot);
  //       texture[i].quad(g);
  //       g.popMatrix();
  //     }
  //     shader().uniform("lighting", 0.2);
  //     shader().uniform("texture", 0.0);
  //     g.popMatrix();
  //   }
  // }
};

int main() {
  MyApp app;
  app.taker.start();
  app.start();
}
