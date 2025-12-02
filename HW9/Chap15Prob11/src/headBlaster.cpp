// headBragg viz

#include "BetheBlochFormula.h"

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/SbColor.h>

#include <QApplication>
#include <QWidget>

#include <cmath>
#include <vector>
#include <iostream>

using namespace Genfun;

constexpr double PI = 3.14159265358979323846;
constexpr double R  = 10.0;

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

double headRadius(double x, double y, double z) {
  double phi    = std::atan2(y, x);
  double phiDeg = phi * 180.0 / PI;
  if (phiDeg < 0.0) phiDeg += 360.0;

  int phiIndex = static_cast<int>(std::round(phiDeg / 30.0)) % 12;
  int zIndex   = static_cast<int>(std::round(z));
  if (zIndex < -15) zIndex = -15;
  if (zIndex >  15) zIndex =  15;

  double RPrime = R - face[phiIndex][zIndex+15];
  return RPrime;
}

bool insideHead(double x, double y, double z) {
  if (z < -15.0 || z > 15.0) return false;
  double rXY = std::sqrt(x*x + y*y);
  double Rs  = headRadius(x,y,z);
  return (rXY <= Rs);
}

double betaGammaFromT(double T, double M) {
  double gamma = 1.0 + T/M;
  double g2    = gamma*gamma;
  double bg2   = g2 - 1.0;
  if (bg2 <= 0.0) return 0.0;
  return std::sqrt(bg2);
}

double rangeFromT0(const BetheBloch &bb, double M,
                   double T0, double dT = 0.2) {
  double x = 0.0;
  for (double T=0.0; T<T0; T+=dT) {
    double Tmid = T + 0.5*dT;
    double bg   = betaGammaFromT(Tmid, M);
    if (bg <= 0.0) continue;
    double S    = bb(bg);
    if (S <= 0.0) continue;
    x += dT / S;
  }
  return x;
}

double findT0ForRange(const BetheBloch &bb, double M,
                      double targetRange,
                      double Tmin = 10.0,
                      double Tmax = 250.0) {
  double fmin = rangeFromT0(bb, M, Tmin) - targetRange;
  double fmax = rangeFromT0(bb, M, Tmax) - targetRange;
  if (fmin > 0.0) return Tmin;
  if (fmax < 0.0) return Tmax;

  for (int it=0; it<40; ++it) {
    double Tmid = 0.5*(Tmin+Tmax);
    double fmid = rangeFromT0(bb, M, Tmid) - targetRange;
    if (fmid * fmin < 0.0) {
      Tmax = Tmid; fmax = fmid;
    } else {
      Tmin = Tmid; fmin = fmid;
    }
  }
  return 0.5*(Tmin + Tmax);
}

struct EnergySample {
  float x, y, z;
  double S;
};

SbColor colorFromS(double S, double Smax) {
  if (Smax <= 0.0) return SbColor(0,0,0);
  double t = std::min(1.0, S / Smax);
  float r = static_cast<float>(t);
  float g = static_cast<float>(std::max(0.0, 1.0 - 2.0*std::abs(t-0.5)));
  float b = static_cast<float>(1.0 - t);
  return SbColor(r,g,b);
}

int main(int argc, char **argv) {

#ifndef __APPLE__
  setenv("QT_QPA_PLATFORM","xcb",0);
#endif

  QApplication app(argc, argv);
  QWidget mainwindow;
  mainwindow.resize(900,900);
  SoQt::init(&mainwindow);

  SoSeparator *root = new SoSeparator;
  root->ref();

  const double degrees = PI/180.0;

  // head mesh
  SoCoordinate3    *coords = new SoCoordinate3;
  SoIndexedFaceSet *faces  = new SoIndexedFaceSet;
  SoMaterial       *headMat= new SoMaterial;
  headMat->diffuseColor.setValue(0.85f, 0.85f, 0.85f);

  unsigned int c = 0, k = 0;
  for (int zIndex=-15; zIndex<=15; ++zIndex) {
    for (int phiIndex=0; phiIndex<=12; ++phiIndex) {
      double phi    = 30.0 * phiIndex * degrees;
      double RPrime = R - face[phiIndex%12][zIndex+15];
      double x      = RPrime * std::cos(phi);
      double y      = RPrime * std::sin(phi);
      double z      = zIndex;

      coords->point.set1Value(c, x, z, y);

      if (zIndex<15 && phiIndex<12) {
        faces->coordIndex.set1Value(k++, c);
        faces->coordIndex.set1Value(k++, c+13);
        faces->coordIndex.set1Value(k++, c+14);
        faces->coordIndex.set1Value(k++, c+1);
        faces->coordIndex.set1Value(k++, SO_END_FACE_INDEX);
      }
      ++c;
    }
  }
  root->addChild(headMat);
  root->addChild(coords);
  root->addChild(faces);

  // pit gland marker
  double pitX = 2.0;
  double pitY = 0.0;
  double pitZ = 1.0;
  double pit[3] = { pitX, pitY, pitZ };

  SoSeparator    *pitNode  = new SoSeparator;
  SoTranslation  *pitTrans = new SoTranslation;
  pitTrans->translation.setValue(pitX, pitZ, pitY);
  SoMaterial     *pitMat   = new SoMaterial;
  pitMat->diffuseColor.setValue(1.0f, 0.0f, 0.0f);
  SoSphere       *pitSphere= new SoSphere;
  pitSphere->radius = 0.4f;
  pitNode->addChild(pitTrans);
  pitNode->addChild(pitMat);
  pitNode->addChild(pitSphere);
  root->addChild(pitNode);

  // bethe-bloch setup
  double Z_water   = 7.42;
  double A_water   = 14.89;
  double I_water   = 7.5e-5;
  double rho_water = 1.0;
  double M_proton  = 938.272;
  int    z_proj    = 1;

  BetheBloch bbWater(Z_water, A_water, I_water,
                     M_proton, z_proj, rho_water);

  // beams + samples
  int    Nbeam = 36;
  double ds    = 0.05;

  SoCoordinate3 *beamCoords = new SoCoordinate3;
  SoLineSet     *beamLines  = new SoLineSet;
  SoMaterial    *beamMat    = new SoMaterial;
  beamMat->diffuseColor.setValue(0.0f, 0.0f, 1.0f);

  std::vector<int> numVerts;
  int bIndex = 0;

  std::vector<EnergySample> samples;

  for (int i=0; i<Nbeam; ++i) {
    double theta = 2.0 * PI * i / Nbeam;
    double ux = std::cos(theta);
    double uy = std::sin(theta);
    double uz = 0.0;

    double s = 0.0;
    double x = pitX;
    double y = pitY;
    double z = pitZ;
    double sEntry = 0.0;
    double xEntry = pitX, yEntry = pitY, zEntry = pitZ;

    for (int step=0; step<2000; ++step) {
      if (!insideHead(x,y,z)) {
        sEntry = s - ds;
        xEntry = pitX - sEntry*ux;
        yEntry = pitY - sEntry*uy;
        zEntry = pitZ - sEntry*uz;
        break;
      }
      s += ds;
      x = pitX - s*ux;
      y = pitY - s*uy;
      z = pitZ - s*uz;
    }
    if (sEntry <= 0.0) continue;

    double L = sEntry;

    double T0 = findT0ForRange(bbWater, M_proton, L);
    std::cout << "Beam " << i
              << " angle(deg)=" << theta*180.0/PI
              << "  L=" << L << " cm, T0=" << T0 << " MeV" << std::endl;

    beamCoords->point.set1Value(bIndex,   xEntry,          zEntry,          yEntry);
    beamCoords->point.set1Value(bIndex+1, pitX,            pitZ,            pitY);
    numVerts.push_back(2);
    bIndex += 2;

    double T = T0;
    double depth = 0.0;
    x = xEntry;
    y = yEntry;
    z = zEntry;

    while (depth <= L && T > 0.0) {
      double bg = betaGammaFromT(T, M_proton);
      if (bg <= 0.0) break;
      double S  = bbWater(bg);

      if (insideHead(x,y,z) && S > 0.0) {
        EnergySample es;
        es.x = static_cast<float>(x);
        es.y = static_cast<float>(y);
        es.z = static_cast<float>(z);
        es.S = S;
        samples.push_back(es);
      }

      depth += ds;
      T     -= S*ds;
      x     += ux*ds;
      y     += uy*ds;
      z     += uz*ds;
    }
  }

  beamLines->numVertices.setValues(0, numVerts.size(), numVerts.data());
  root->addChild(beamMat);
  root->addChild(beamCoords);
  root->addChild(beamLines);

  // energy point cloud
  SoPointSet       *energyPoints = new SoPointSet;
  SoVertexProperty *energyVP     = new SoVertexProperty;

  double Smax = 0.0;
  for (const auto &smp : samples) {
    if (smp.S > Smax) Smax = smp.S;
  }

  for (std::size_t i=0; i<samples.size(); ++i) {
    const auto &smp = samples[i];

    energyVP->vertex.set1Value(
      static_cast<int>(i),
      smp.x,
      smp.z,
      smp.y
    );

    SbColor col = colorFromS(smp.S, Smax);
    uint32_t rgba = col.getPackedValue(0);
    energyVP->orderedRGBA.set1Value(static_cast<int>(i), rgba);
  }

  energyPoints->vertexProperty = energyVP;
  root->addChild(energyPoints);

  // viewer stuff
  SoQtExaminerViewer *ev = new SoQtExaminerViewer(&mainwindow);
  ev->setSceneGraph(root);
  ev->setDoubleBuffer(true);
  ev->show();

  SoQt::show(&mainwindow);
  SoQt::mainLoop();

  delete ev;
  root->unref();
  return 0;
}