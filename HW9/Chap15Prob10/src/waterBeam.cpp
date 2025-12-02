#include "BetheBlochFormula.h"
#include "QatGenericFunctions/InterpolatingFunction.h"

#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotKey.h"

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

int main (int argc, char * * argv) {

  std::string usage = std::string("usage: ") + argv[0];
  if (argc != 1) {
    std::cout << usage << std::endl;
  }

  using namespace Genfun;

  // water + particle params
  double Z_water   = 7.42;
  double A_water   = 14.89;
  double I_water   = 7.5e-5;
  double rho_water = 1.0;

  double M_proton  = 938.272;
  double M_pion    = 139.57;
  int    z_proj    = 1;

  BetheBloch dEdxWater_p(Z_water, A_water, I_water,
                         M_proton, z_proj, rho_water);

  BetheBloch dEdxWater_pi(Z_water, A_water, I_water,
                          M_pion, z_proj, rho_water);

  // energy loss integration setup
  double T0   = 150.0;
  double Tmin = 0.5;
  double dx   = 0.01;
  unsigned maxSteps = 500000;

  auto betaGammaFromT = [](double T, double M) -> double {
    double gamma = 1.0 + T / M;
    double g2    = gamma * gamma;
    double bg2   = g2 - 1.0;
    if (bg2 < 0.0) bg2 = 0.0;
    return std::sqrt(bg2);
  };

  // integrate one beam
  auto integrateBeam = [&](const BetheBloch &bb,
                           double M,
                           std::vector<double> &xVals,
                           std::vector<double> &sVals) {
    xVals.clear();
    sVals.clear();
    xVals.reserve(maxSteps + 2);
    sVals.reserve(maxSteps + 2);

    double T = T0;
    double x = 0.0;

    for (unsigned i = 0; i < maxSteps && T > Tmin; ++i) {
      double bg   = betaGammaFromT(T, M);
      double S    = bb(bg);

      xVals.push_back(x);
      sVals.push_back(S);

      double dE = S * dx;
      T -= dE;
      if (T <= 0.0) {
        T = 0.0;
        break;
      }
      x += dx;
    }

    if (!xVals.empty()) {
      double xLast = xVals.back();
      double xEnd  = xLast + dx;
      xVals.push_back(xEnd);
      sVals.push_back(0.0);
    }
  };

  std::vector<double> xP,  sP;
  std::vector<double> xPi, sPi;

  integrateBeam(dEdxWater_p,  M_proton, xP,  sP);
  integrateBeam(dEdxWater_pi, M_pion,   xPi, sPi);

  if (xP.size() < 2 || xPi.size() < 2) {
    std::cerr << "Not enough points — check T0, Tmin, and dx.\n";
    return 1;
  }

  // spline fits
  InterpolatingFunction braggP (Genfun::CUBIC_SPLINE);
  InterpolatingFunction braggPi(Genfun::CUBIC_SPLINE);

  double xMax = std::max(xP.back(), xPi.back());
  double yMax = 0.0;

  for (std::size_t i = 0; i < xP.size(); ++i) {
    braggP.addPoint(xP[i], sP[i]);
    if (sP[i] > yMax) yMax = sP[i];
  }
  for (std::size_t i = 0; i < xPi.size(); ++i) {
    braggPi.addPoint(xPi[i], sPi[i]);
    if (sPi[i] > yMax) yMax = sPi[i];
  }
  yMax *= 1.2;

  // plot setup
  QApplication app(argc, argv);
  
  QMainWindow window;
  QToolBar *toolBar = window.addToolBar("Tools");
  QAction  *quitAction = toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(xMax);
  rect.setYmin(0.0);
  rect.setYmax(yMax);
  
  PlotView view(rect);
  view.setLogX(false);
  view.setLogY(false);
  view.setBox(false);
  view.setGrid(true);
  view.setFixedWidth(900);
  view.setFixedHeight(700);
  window.setCentralWidget(&view);

  PlotFunction1D pBraggP (braggP);
  PlotFunction1D pBraggPi(braggPi);

  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);

    prop.pen.setColor(Qt::red);
    pBraggP.setProperties(prop);
    view.add(&pBraggP);

    prop.pen.setColor(Qt::blue);
    pBraggPi.setProperties(prop);
    view.add(&pBraggPi);
  }

  PlotKey *key = new PlotKey(0.05 * xMax, 0.90 * yMax);
  key->add(&pBraggP,  "Proton (150 MeV)");
  key->add(&pBraggPi, "π^{±} (150 MeV)");
  view.add(key);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
              << PlotStream::Center() 
              << PlotStream::Family("Sans Serif") 
              << PlotStream::Size(16)
              << "Bragg peaks in water (T₀ = 150 MeV)"
              << PlotStream::EndP();
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(16)
               << "Depth x [cm]"
               << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(16)
               << "-dE/dx [MeV/cm]"
               << PlotStream::EndP();
  
  view.show();
  window.show();
  app.exec();
  return 0;
}