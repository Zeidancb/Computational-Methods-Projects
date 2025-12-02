// SoQt includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
// Coin includes
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTranslation.h>
// Inventor includes:
#include <QApplication>
#include <QWidget>
//------------------------------------------------

SoTranslation *gLeftBallT  = nullptr;
SoTranslation *gRightBallT = nullptr;

SoCoordinate3 *gLeftStrL   = nullptr;
SoCoordinate3 *gLeftStrR   = nullptr;
SoCoordinate3 *gRightStrL  = nullptr;
SoCoordinate3 *gRightStrR  = nullptr;

double gXLeft  = 0.0;
double gXRight = 0.0;
double gYTop   = 0.0;
double gL      = 1.0;
double gAmax   = M_PI_2;

double gPhase = 0.0;
double gDPhase = 0.01;

static inline double halfcycle_angle(double tau) {
  return gAmax * std::sin(M_PI * tau);
}

static void cradleTick(void*, SoSensor*) {
  gPhase += gDPhase;
  if (gPhase >= 2.0) gPhase -= 2.0;

  auto setBall = [](SoTranslation* T, SoCoordinate3* SL, SoCoordinate3* SR,
                    double x0, double ang, int sideSign){
    if (!T || !SL || !SR) return;
    const double bx = x0 + sideSign * gL * std::sin(ang);
    const double by = gYTop - gL * std::cos(ang);
    T->translation.setValue(bx, by, 0.0f);
    SL->point.set1Value(1, bx, by, 0.0f);
    SR->point.set1Value(1, bx, by, 0.0f);
  };

  if (gPhase < 1.0) {
    const double tau = gPhase;
    const double ang = halfcycle_angle(tau);
    setBall(gLeftBallT,  gLeftStrL,  gLeftStrR,  gXLeft,  ang, -1);
    setBall(gRightBallT, gRightStrL, gRightStrR, gXRight, 0.0, +1);
  } else {
    const double tau = gPhase - 1.0;
    const double ang = halfcycle_angle(tau);
    setBall(gLeftBallT,  gLeftStrL,  gLeftStrR,  gXLeft,  0.0, -1);
    setBall(gRightBallT, gRightStrL, gRightStrR, gXRight, ang, +1);
  }
}

int main(int argc, char **argv)
{

  // Initialize the Qt system:
  QApplication app(argc, argv);
  
  // Make a main window:
  QWidget mainwin;
  mainwin.resize(600, 600);

  // Initialize SoQt 
  SoQt::init(&mainwin);
  
  // The root of a scene graph
  SoSeparator *root = new SoSeparator;
  root->ref();

  //---------------------------
  SoMaterial *metal = new SoMaterial;
  metal->diffuseColor.setValue(0.8f, 0.8f, 0.8f);
  metal->specularColor.setValue(1.0f, 1.0f, 1.0f);
  metal->shininess = 0.9f;
  root->addChild(metal);

  float baseCenterY = -1.0f;
  float baseHalfH   = 0.1f;
  float baseTopY    = baseCenterY + baseHalfH;
  {
    SoSeparator *baseSep = new SoSeparator;
    SoTransform *baseXf = new SoTransform;
    baseXf->translation.setValue(0, baseCenterY, 0);
    SoCube *base = new SoCube;
    base->width  = 4.0f;
    base->height = 0.2f;
    base->depth  = 2.0f;
    baseSep->addChild(baseXf);
    baseSep->addChild(base);
    root->addChild(baseSep);
  }

  float pillarHeight = 2.0f;
  float pillarOffsetX = 1.8f;
  float pillarOffsetZ = 0.8f;

  for (int i = 0; i < 4; i++) {
    SoSeparator *pillarSep = new SoSeparator;
    SoTransform *pillarXf = new SoTransform;
    pillarXf->translation.setValue(
      (i < 2 ? -pillarOffsetX/2.0f : pillarOffsetX/2.0f),
      baseTopY + pillarHeight/2.0f,
      (i % 2 == 0 ? -pillarOffsetZ/2.0f : pillarOffsetZ/2.0f)
    );
    SoCylinder *pillar = new SoCylinder;
    pillar->height = pillarHeight;
    pillar->radius = 0.05f;
    pillarSep->addChild(pillarXf);
    pillarSep->addChild(pillar);
    root->addChild(pillarSep);
  }

  float barLength = 1.8f;
  float barY = baseTopY + 0.90f * pillarHeight;
  float zLeftBar  = -pillarOffsetZ/2.0f;
  float zRightBar =  pillarOffsetZ/2.0f;

  for (int i = 0; i < 2; i++) {
    SoSeparator *barSep = new SoSeparator;
    SoTransform *barXf = new SoTransform;
    barXf->translation.setValue(0.0f, barY, (i == 0 ? zLeftBar : zRightBar));
    barXf->rotation.setValue(SbVec3f(0, 0, 1), M_PI_2);
    SoCylinder *bar = new SoCylinder;
    bar->height = barLength;
    bar->radius = 0.04f;
    barSep->addChild(barXf);
    barSep->addChild(bar);
    root->addChild(barSep);
  }

  int   numBalls    = 4;
  float ballRadius  = 0.12f;
  float spacing     = 2.0f * ballRadius;
  float yBallCenter = baseTopY + 0.20f * pillarHeight;
  float stringLen   = barY - yBallCenter;
  float topSpread   = 0.5f * ballRadius;

  gYTop = barY; gL = stringLen;

  for (int i = 0; i < numBalls; i++) {
    const float xCenter = -((numBalls - 1) * spacing) / 2.0f + i * spacing;

    SoSeparator *leftStrSep = new SoSeparator;
    SoCoordinate3 *leftStrCoords = new SoCoordinate3;
    leftStrCoords->point.set1Value(0, xCenter - topSpread, barY, zLeftBar);
    leftStrCoords->point.set1Value(1, xCenter, yBallCenter, 0.0f);
    SoLineSet *leftLine = new SoLineSet; leftLine->numVertices.set1Value(0, 2);
    leftStrSep->addChild(leftStrCoords);
    leftStrSep->addChild(leftLine);
    root->addChild(leftStrSep);

    SoSeparator *rightStrSep = new SoSeparator;
    SoCoordinate3 *rightStrCoords = new SoCoordinate3;
    rightStrCoords->point.set1Value(0, xCenter + topSpread, barY, zRightBar);
    rightStrCoords->point.set1Value(1, xCenter, yBallCenter, 0.0f);
    SoLineSet *rightLine = new SoLineSet; rightLine->numVertices.set1Value(0, 2);
    rightStrSep->addChild(rightStrCoords);
    rightStrSep->addChild(rightLine);
    root->addChild(rightStrSep);

    SoSeparator *ballSep = new SoSeparator;
    SoTranslation *ballT = new SoTranslation;
    ballT->translation.setValue(xCenter, yBallCenter, 0.0f);
    SoSphere *ball = new SoSphere; ball->radius = ballRadius;
    ballSep->addChild(ballT);
    ballSep->addChild(ball);
    root->addChild(ballSep);

    if (i == 0) {
      gLeftBallT = ballT;  gLeftStrL = leftStrCoords;  gLeftStrR = rightStrCoords;  gXLeft  = xCenter;
    }
    if (i == numBalls - 1) {
      gRightBallT = ballT; gRightStrL = leftStrCoords; gRightStrR = rightStrCoords; gXRight = xCenter;
    }
  }

  {
    SoTimerSensor *timer = new SoTimerSensor;
    timer->setFunction(cradleTick);
    timer->setInterval(0.016);
    timer->schedule();
  }
  //---------------------------

  // Initialize an examiner viewer:
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer(&mainwin);
  eviewer->setSceneGraph(root);
  eviewer->show();
  
  // Pop up the main window.
  SoQt::show(&mainwin);

  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  root->unref();
}