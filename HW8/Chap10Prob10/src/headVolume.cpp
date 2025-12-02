
// SoQt includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
// Coin includes
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <QApplication>
#include <QWidget>

#include <random>
#include <iostream>
#include <cmath>


int main(int argc, char ** argv)
{
#ifndef __APPLE__
  setenv ("QT_QPA_PLATFORM","xcb",0);
#endif
  
  QApplication app(argc, argv);

  QWidget mainwindow;
  mainwindow.resize(400, 400);

  SoQt::init(&mainwindow);
  
  // Set the main node for the "scene graph" 
  SoSeparator *root = new SoSeparator;
  root->ref();

  SoCoordinate3 *coords=new SoCoordinate3;

  constexpr double PI = 3.14159265358979323846;
  constexpr double degrees = PI/180.0;
  constexpr double R=10.0;
  unsigned int c{0},k{0};

  double face[12][31]{
    {6, 6, 6, 6, 4, 4, 4, 3.6, 3.3, 1.7,
     1.8, 2.3, 1.9, 1.7, 1.9, 0, 0, 0, 0, 0.2,
     1.4, 1.5, 1.5, 0.9, 0.9, 0.9, 1, 1.2, 1.5, 2.3,
     10},
    {4.5, 4.5, 4.5, 4.5, 2.5, 2.5, 2.5, 2.1, 1.8, 0.2,
     0.3, 0.8, 0.4, 0.2, 0.4, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8,
     8.5},
    {4, 4, 4, 4, 3, 3, 3, 2.5, 2, 1.5,
     1.5, 1.5, 1.5, 1.5, 1.5, 1.3, 1.3, 1.3, 1.3, 1.5,
     2, 2.5, 3, 3, 3, 3, 3.5, 4, 4.5, 5,
     6},
    {4, 4, 4, 4, 3, 3, 3, 2.5, 2, 1.5,
     1.5, 1.5, 1.5, 1.5, 1.5, 1.3, 1.3, 1.3, 1.3, 1.5,
     2, 2.5, 3, 3, 3, 3, 3.5, 4, 4.5, 5,
     6},
    {4, 4, 4, 4, 3, 3, 3, 2.5, 2, 1.5,
     1.5, 1.5, 1.5, 1.5, 1.5, 1.3, 1.3, 1.3, 1.3, 1.5,
     2, 2.5, 3, 3, 3, 3, 3.5, 4, 4.5, 5,
     6},
    {4, 4, 4, 4, 3, 3, 3, 2.5, 2, 1.5,
     1.5, 1.5, 1.5, 1.5, 1.5, 1.3, 1.3, 1.3, 1.3, 1.5,
     2, 2.5, 3, 3, 3, 3, 3.5, 4, 4.5, 5,
     6},
    {4, 4, 4, 4, 3, 3, 3, 2.5, 2, 1.5,
     1.5, 1.5, 1.5, 1.5, 1.5, 1.3, 1.3, 1.3, 1.3, 1.5,
     2, 2.5, 3, 3, 3, 3, 3.5, 4, 4.5, 5,
     6},
    {4, 4, 4, 4, 3, 3, 3, 2.5, 2, 1.5,
     1.5, 1.5, 1.5, 1.5, 1.5, 1.3, 1.3, 1.3, 1.3, 1.5,
     2, 2.5, 3, 3, 3, 3, 3.5, 4, 4.5, 5,
     6},
    {4, 4, 4, 4, 3, 3, 3, 2.5, 2, 1.5,
     1.5, 1.5, 1.5, 1.5, 1.5, 1.3, 1.3, 1.3, 1.3, 1.5,
     2, 2.5, 3, 3, 3, 3, 3.5, 4, 4.5, 5,
     6},
    {4, 4, 4, 4, 3, 3, 3, 2.5, 2, 1.5,
     1.5, 1.5, 1.5, 1.5, 1.5, 1.3, 1.3, 1.3, 1.3, 1.5,
     2, 2.5, 3, 3, 3, 3, 3.5, 4, 4.5, 5,
     6},
    {4.5, 4.5, 4.5, 4.5, 2.5, 2.5, 2.5, 2.1, 1.8, 0.2,
     0.3, 0.8, 0.4, 0.2, 0.4, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8,
     8.5},
    {6, 6, 6, 6, 4, 4, 4, 3.6, 3.3, 1.7,
     1.8, 2.3, 1.9, 1.7, 1.9, 0, 0, 0, 0, 0.2,
     1.4, 1.5, 1.5, 0.9, 0.9, 0.9, 1, 1.2, 1.5, 2.3,
     10}
  };

  SoIndexedFaceSet *faceSet=new SoIndexedFaceSet;
  
  for (int zIndex=-15;zIndex<=15;zIndex++) {
    for (int phiIndex=0;phiIndex<=12; phiIndex++) {
      double phi=30*phiIndex*degrees;
      double RPrime=R-face[phiIndex%12][zIndex+15];
      double x=RPrime*std::cos(phi);
      double y=RPrime*std::sin(phi);
      double z=zIndex;
      coords->point.set1Value(c, x,z,y);
      // Indexing is a little tricky.  This sets the order of the vertices
      // so that surface normals to each face points outward.  The direction
      // of the normal determines how light is reflected from each face.
      if (zIndex<15 && phiIndex<12) {
        faceSet->coordIndex.set1Value(k++,c);
        faceSet->coordIndex.set1Value(k++,c+13);
        faceSet->coordIndex.set1Value(k++,c+14);
        faceSet->coordIndex.set1Value(k++,c+1);
        faceSet->coordIndex.set1Value(k++,SO_END_FACE_INDEX);
      }
      c++;
    }
  }

  SoDrawStyle *drawStyle=new SoDrawStyle;
  drawStyle->style=SoDrawStyle::LINES;

  SoLightModel *lightModel=new SoLightModel;
  lightModel->model=SoLightModel::BASE_COLOR;
  
  root->addChild(coords);
  root->addChild(faceSet);
  
  // Monte Carlo Integrator
  {

    double xmin = -R;
    double xmax =  R;
    double ymin = -R;
    double ymax =  R;
    double zmin = -15.0;
    double zmax =  15.0;

    double boxVolume = (xmax - xmin) * (ymax - ymin) * (zmax - zmin);

    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_real_distribution<double> distX(xmin, xmax);
    std::uniform_real_distribution<double> distY(ymin, ymax);
    std::uniform_real_distribution<double> distZ(zmin, zmax);

    auto isInside = [&](double x, double y, double z) -> bool {
      double r   = std::sqrt(x*x + y*y);
      double phi = std::atan2(y, x);

      double phiDeg = phi * 180.0 / PI;
      if (phiDeg < 0.0) phiDeg += 360.0;

      int phiIndex = static_cast<int>(std::round(phiDeg / 30.0)) % 12;

      int zIndex = static_cast<int>(std::round(z));
      if (zIndex < -15) zIndex = -15;
      if (zIndex >  15) zIndex =  15;

      double RPrime = R - face[phiIndex][zIndex + 15];

      return (r <= RPrime);
    };

    std::size_t Nsamples = 500000;
    std::size_t insideCount = 0;

    for (std::size_t i = 0; i < Nsamples; ++i) {
      double x = distX(rng);
      double y = distY(rng);
      double z = distZ(rng);
      if (isInside(x, y, z)) {
        insideCount++;
      }
    }

    double fraction = static_cast<double>(insideCount) / static_cast<double>(Nsamples);
    double volume   = fraction * boxVolume;

    std::cout << "Monte Carlo estimate of head volume: "
              << volume << " (model units^3)" << std::endl;
  }
  
  // Init the viewer and get a pointer to it
  SoQtExaminerViewer *ev = new SoQtExaminerViewer(&mainwindow);
  ev->setDoubleBuffer(false);
  
  // Set the main node as content of the window and show it
  ev->setSceneGraph(root);
  ev->show();
  
  // Start the windowing system and show our window 
  SoQt::show(&mainwindow);
  
  // Loop until exit.
  SoQt::mainLoop();

  // When exiting the main loop,
  // delete the viewer and leave the handle to the main node 
  delete ev;
  root->unref();

  return 0;
}