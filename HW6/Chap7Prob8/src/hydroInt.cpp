#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotKey.h"
#include "QatGenericFunctions/InterpolatingFunction.h"

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <random>
#include <string>

using namespace Genfun;

int main(int argc, char **argv) {

  // monte carlo samples
  std::size_t Nsamples = (argc > 1) ? std::strtoull(argv[1], nullptr, 10)
                                    : 200000;

  // d range
  const double dMin = 0.5;
  const double dMax = 8.0;
  const int    Nd   = 25;

  const double Rmax = 8.0; // box is [-rmax,rmax]^3

  const double invSqrtPi  = 1.0 / std::sqrt(3.14159265358979323846);
  const double cubeVolume = std::pow(2.0 * Rmax, 3);

  // rng
  std::random_device rd;
  std::mt19937_64 rng(rd());
  std::uniform_real_distribution<double> uni(-Rmax, Rmax);

  // simple monte carlo integrator
  auto mcIntegral = [&](auto &&f, std::size_t N)->double {
    double sum = 0.0;
    for (std::size_t i = 0; i < N; ++i) {
      double x = uni(rng);
      double y = uni(rng);
      double z = uni(rng);
      sum += f(x,y,z);
    }
    return cubeVolume * sum / double(N);
  };

  // data containers
  std::vector<double> dVals;
  std::vector<double> NsVals, NaVals;
  std::vector<double> EsVals, EaVals;

  dVals.reserve(Nd);
  NsVals.reserve(Nd);
  NaVals.reserve(Nd);
  EsVals.reserve(Nd);
  EaVals.reserve(Nd);

  // loop over separations
  for (int i = 0; i < Nd; ++i) {
    double d = dMin + (dMax - dMin) * i / double(Nd - 1);
    double z1Shift =  d/2.0;
    double z2Shift = -d/2.0;

    // overlap s(d)
    auto phi1phi2 = [&](double x, double y, double z)->double {
      double z1 = z - z1Shift;
      double z2 = z - z2Shift;
      double r1 = std::sqrt(x*x + y*y + z1*z1);
      double r2 = std::sqrt(x*x + y*y + z2*z2);
      double phi1 = invSqrtPi * std::exp(-r1);
      double phi2 = invSqrtPi * std::exp(-r2);
      return phi1 * phi2;
    };

    // j(d)
    auto phi1sq_over_r2 = [&](double x, double y, double z)->double {
      double z1 = z - z1Shift;
      double z2 = z - z2Shift;
      double r1 = std::sqrt(x*x + y*y + z1*z1);
      double r2 = std::sqrt(x*x + y*y + z2*z2);
      double phi1 = invSqrtPi * std::exp(-r1);
      double phi1sq = phi1 * phi1;
      return -phi1sq / r2;
    };

    // k(d)
    auto phi1phi2_over_r2 = [&](double x, double y, double z)->double {
      double z1 = z - z1Shift;
      double z2 = z - z2Shift;
      double r1 = std::sqrt(x*x + y*y + z1*z1);
      double r2 = std::sqrt(x*x + y*y + z2*z2);
      double phi1 = invSqrtPi * std::exp(-r1);
      double phi2 = invSqrtPi * std::exp(-r2);
      return -(phi1 * phi2) / r2;
    };

    // monte carlo estimates
    double S = mcIntegral(phi1phi2,          Nsamples);
    double J = mcIntegral(phi1sq_over_r2,    Nsamples);
    double K = mcIntegral(phi1phi2_over_r2,  Nsamples);

    // normalization
    double Ns = 1.0 / std::sqrt(2.0 * (1.0 + S));
    double Na = 1.0 / std::sqrt(2.0 * (1.0 - S));

    // energies (electronic)
    double Es = (-0.5 + J - 0.5*S + K) / (1.0 + S);
    double Ea = (-0.5 + J + 0.5*S - K) / (1.0 - S);

    dVals.push_back(d);
    NsVals.push_back(Ns);
    NaVals.push_back(Na);
    EsVals.push_back(Es);
    EaVals.push_back(Ea);

    std::cout << "d=" << d
              << "  S=" << S
              << "  Ns=" << Ns
              << "  Na=" << Na
              << "  Es=" << Es
              << "  Ea=" << Ea
              << std::endl;
  }

  // interpolating functions
  InterpolatingFunction NsFunc{CUBIC_SPLINE};
  InterpolatingFunction NaFunc{CUBIC_SPLINE};
  InterpolatingFunction EsFunc{CUBIC_SPLINE};
  InterpolatingFunction EaFunc{CUBIC_SPLINE};

  for (std::size_t i = 0; i < dVals.size(); ++i) {
    NsFunc.addPoint(dVals[i], NsVals[i]);
    NaFunc.addPoint(dVals[i], NaVals[i]);
    EsFunc.addPoint(dVals[i], EsVals[i]);
    EaFunc.addPoint(dVals[i], EaVals[i]);
  }

  // qt / qat
  QApplication app(argc, argv);

  // normalization window
  QMainWindow normWindow;
  QToolBar *normToolbar = normWindow.addToolBar("Tools");
  QAction  *normQuit    = normToolbar->addAction("Quit");
  normQuit->setShortcut(QKeySequence("q"));
  QObject::connect(normQuit, SIGNAL(triggered()), &app, SLOT(quit()));

  PRectF normRect;
  normRect.setXmin(dMin);
  normRect.setXmax(dMax);
  normRect.setYmin(0.0);
  normRect.setYmax(1.1);

  PlotView normView(normRect);
  normView.setXZero(false);
  normView.setYZero(false);
  normView.setGrid(true);
  normView.setFixedWidth(800);
  normView.setFixedHeight(600);
  normWindow.setCentralWidget(&normView);

  PlotFunction1D P_Ns(NsFunc);
  PlotFunction1D P_Na(NaFunc);

  {
    PlotFunction1D::Properties p;
    p.pen.setWidth(2);

    p.pen.setColor(Qt::red);
    P_Ns.setProperties(p);
    normView.add(&P_Ns);

    p.pen.setColor(Qt::blue);
    P_Na.setProperties(p);
    normView.add(&P_Na);
  }

  PlotKey *normKey = new PlotKey(dMin + 0.1*(dMax-dMin), 1.05);
  normKey->add(&P_Ns, "N_s(d)");
  normKey->add(&P_Na, "N_a(d)");
  normView.add(normKey);

  PlotStream normTitle(normView.titleTextEdit());
  normTitle << PlotStream::Clear()
            << PlotStream::Center()
            << PlotStream::Size(16)
            << PlotStream::Family("Sans Serif")
            << "normalization vs d"
            << PlotStream::EndP();

  PlotStream normX(normView.xLabelTextEdit());
  normX << PlotStream::Clear()
        << PlotStream::Center()
        << PlotStream::Family("Sans Serif")
        << PlotStream::Size(14)
        << "d (bohr)"
        << PlotStream::EndP();

  PlotStream normY(normView.yLabelTextEdit());
  normY << PlotStream::Clear()
        << PlotStream::Center()
        << PlotStream::Family("Sans Serif")
        << PlotStream::Size(14)
        << "N_s, N_a"
        << PlotStream::EndP();

  normWindow.show();

  // energy window
  QMainWindow energyWindow;
  QToolBar *energyToolbar = energyWindow.addToolBar("Tools");
  QAction  *energyQuit    = energyToolbar->addAction("Quit");
  energyQuit->setShortcut(QKeySequence("e"));
  QObject::connect(energyQuit, SIGNAL(triggered()), &app, SLOT(quit()));

  double Emin = -1.5;
  double Emax =  0.5;

  PRectF energyRect;
  energyRect.setXmin(dMin);
  energyRect.setXmax(dMax);
  energyRect.setYmin(Emin);
  energyRect.setYmax(Emax);

  PlotView energyView(energyRect);
  energyView.setXZero(false);
  energyView.setYZero(true);
  energyView.setGrid(true);
  energyView.setFixedWidth(800);
  energyView.setFixedHeight(600);
  energyWindow.setCentralWidget(&energyView);

  PlotFunction1D P_Es(EsFunc);
  PlotFunction1D P_Ea(EaFunc);

  {
    PlotFunction1D::Properties p;
    p.pen.setWidth(2);

    p.pen.setColor(Qt::darkGreen);
    P_Es.setProperties(p);
    energyView.add(&P_Es);

    p.pen.setColor(Qt::magenta);
    P_Ea.setProperties(p);
    energyView.add(&P_Ea);
  }

  PlotKey *energyKey = new PlotKey(dMin + 0.1*(dMax-dMin),
                                   Emax - 0.1*(Emax-Emin));
  energyKey->add(&P_Es, "E_s(d)");
  energyKey->add(&P_Ea, "E_a(d)");
  energyView.add(energyKey);

  PlotStream energyTitle(energyView.titleTextEdit());
  energyTitle << PlotStream::Clear()
              << PlotStream::Center()
              << PlotStream::Size(16)
              << PlotStream::Family("Sans Serif")
              << "h2+ energies vs d"
              << PlotStream::EndP();

  PlotStream energyX(energyView.xLabelTextEdit());
  energyX << PlotStream::Clear()
          << PlotStream::Center()
          << PlotStream::Family("Sans Serif")
          << PlotStream::Size(14)
          << "d (bohr)"
          << PlotStream::EndP();

  PlotStream energyY(energyView.yLabelTextEdit());
  energyY << PlotStream::Clear()
          << PlotStream::Center()
          << PlotStream::Family("Sans Serif")
          << PlotStream::Size(14)
          << "E (hartree)"
          << PlotStream::EndP();

  energyWindow.show();

  app.exec();
  return 0;
}