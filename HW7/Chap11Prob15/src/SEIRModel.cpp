#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotKey.h"
#include "QatGenericFunctions/InterpolatingFunction.h"

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <libgen.h>

int main (int argc, char * * argv) {

  std::string usage = std::string("usage: ") + argv[0];
  if (argc != 1) {
    std::cout << usage << std::endl;
  }

  QApplication app(argc, argv);

  // log plot code

  QMainWindow window;
  QToolBar *toolBar = window.addToolBar("Tools");
  QAction  *nextAction = toolBar->addAction("Next");

  nextAction->setShortcut(QKeySequence("n"));
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));

  double Tmax = 4000.0;   // integrate / plot up to 4000 days

  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(Tmax);
  rect.setYmin(-8.0);
  rect.setYmax(0.5);

  PlotView view(rect);
  view.setXZero(false);
  view.setYZero(false);
  view.setGrid(false);
  view.setFixedWidth(900);
  view.setFixedHeight(700);
  window.setCentralWidget(&view);

  // params

  const double beta   = 0.2;
  const double gamma  = 0.1;
  const double bRate  = 1.0e-4;  // birth rate
  const double dRate  = 1.0e-4;  // death rate
  const double alpha  = 0.1;     // E -> I
  const double nu     = 1.0e-3;  // R -> S

  double N0 = 1.0;
  double I  = 1.0e-6;
  double E  = 0.0;
  double R  = 0.0;
  double S  = N0 - I;

  double dt       = 1.0;
  int    nSteps   = int(Tmax / dt);
  double t        = 0.0;

  Genfun::InterpolatingFunction logIFrac(Genfun::CUBIC_SPLINE);

  Genfun::InterpolatingFunction SofT(Genfun::CUBIC_SPLINE);
  Genfun::InterpolatingFunction EofT(Genfun::CUBIC_SPLINE);
  Genfun::InterpolatingFunction IofT(Genfun::CUBIC_SPLINE);
  Genfun::InterpolatingFunction RofT(Genfun::CUBIC_SPLINE);

  auto derivatives = [&](double S, double E, double I, double R,
                         double &dSdt, double &dEdt,
                         double &dIdt, double &dRdt) {

    double N = S + E + I + R;
    double lambda = beta * I / N;

    dSdt = bRate * N - lambda * S - dRate * S + nu * R;
    dEdt = lambda * S - alpha * E - dRate * E;
    dIdt = alpha * E - gamma * I - dRate * I;
    dRdt = gamma * I - dRate * R - nu * R;
  };

  // RK4

  for (int n = 0; n <= nSteps; ++n) {

    double N = S + E + I + R;

    double fracI = I / N;
    if (fracI < 1.0e-12) fracI = 1.0e-12;
    double yLog = std::log10(fracI);
    logIFrac.addPoint(t, yLog);

    SofT.addPoint(t, S);
    EofT.addPoint(t, E);
    IofT.addPoint(t, I);
    RofT.addPoint(t, R);

    double k1S, k1E, k1I, k1R;
    double k2S, k2E, k2I, k2R;
    double k3S, k3E, k3I, k3R;
    double k4S, k4E, k4I, k4R;

    derivatives(S, E, I, R, k1S, k1E, k1I, k1R);

    derivatives(S + 0.5*dt*k1S,
                E + 0.5*dt*k1E,
                I + 0.5*dt*k1I,
                R + 0.5*dt*k1R,
                k2S, k2E, k2I, k2R);

    derivatives(S + 0.5*dt*k2S,
                E + 0.5*dt*k2E,
                I + 0.5*dt*k2I,
                R + 0.5*dt*k2R,
                k3S, k3E, k3I, k3R);

    derivatives(S + dt*k3S,
                E + dt*k3E,
                I + dt*k3I,
                R + dt*k3R,
                k4S, k4E, k4I, k4R);

    S += (dt/6.0) * (k1S + 2.0*k2S + 2.0*k3S + k4S);
    E += (dt/6.0) * (k1E + 2.0*k2E + 2.0*k3E + k4E);
    I += (dt/6.0) * (k1I + 2.0*k2I + 2.0*k3I + k4I);
    R += (dt/6.0) * (k1R + 2.0*k2R + 2.0*k3R + k4R);

    t += dt;
  }

  // log plot window

  PlotFunction1D P_logIFrac(logIFrac);
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(2);
    prop.pen.setColor(Qt::red);
    P_logIFrac.setProperties(prop);
  }
  view.add(&P_logIFrac);

  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
              << PlotStream::Center()
              << PlotStream::Family("Sans Serif")
              << PlotStream::Size(16)
              << "SEIR model: log10(Infected fraction I/N)"
              << PlotStream::EndP();

  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(16)
               << "Time (days)"
               << PlotStream::EndP();

  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(16)
               << "log10(I/N)"
               << PlotStream::EndP();

  // Plotted Linear scale also as a sanity check

  QMainWindow window2;
  QToolBar *toolBar2 = window2.addToolBar("Tools");
  QAction  *nextAction2 = toolBar2->addAction("Next");
  nextAction2->setShortcut(QKeySequence("n"));
  QObject::connect(nextAction2, SIGNAL(triggered()), &app, SLOT(quit()));

  PRectF rect2;
  rect2.setXmin(0.0);
  rect2.setXmax(Tmax);
  rect2.setYmin(0.0);
  rect2.setYmax(1.1 * N0); 

  PlotView view2(rect2);
  view2.setXZero(false);
  view2.setYZero(false);
  view2.setGrid(true);
  view2.setFixedWidth(900);
  view2.setFixedHeight(700);
  window2.setCentralWidget(&view2);

  PlotFunction1D P_S(SofT);
  PlotFunction1D P_E(EofT);
  PlotFunction1D P_I(IofT);
  PlotFunction1D P_R(RofT);

  {
    PlotFunction1D::Properties prop;

    prop.pen.setWidth(2);
    prop.pen.setColor(Qt::blue);   // S
    P_S.setProperties(prop);
    view2.add(&P_S);

    prop.pen.setColor(Qt::magenta); // E
    P_E.setProperties(prop);
    view2.add(&P_E);

    prop.pen.setColor(Qt::red);    // I
    P_I.setProperties(prop);
    view2.add(&P_I);

    prop.pen.setColor(Qt::darkGreen); // R
    P_R.setProperties(prop);
    view2.add(&P_R);
  }

  PlotKey *key = new PlotKey(0.05 * Tmax, 0.95 * rect2.ymax());
  key->add(&P_S, "S(t)");
  key->add(&P_E, "E(t)");
  key->add(&P_I, "I(t)");
  key->add(&P_R, "R(t)");
  view2.add(key);

  PlotStream titleStream2(view2.titleTextEdit());
  titleStream2 << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(16)
               << "SEIR model: compartment populations"
               << PlotStream::EndP();

  PlotStream xLabelStream2(view2.xLabelTextEdit());
  xLabelStream2 << PlotStream::Clear()
                << PlotStream::Center()
                << PlotStream::Family("Sans Serif")
                << PlotStream::Size(16)
                << "Time (days)"
                << PlotStream::EndP();

  PlotStream yLabelStream2(view2.yLabelTextEdit());
  yLabelStream2 << PlotStream::Clear()
                << PlotStream::Center()
                << PlotStream::Family("Sans Serif")
                << PlotStream::Size(16)
                << "Population (units of N)"
                << PlotStream::EndP();

  // display

  view.show();
  window.show();

  view2.show();
  window2.show();

  app.exec();

  return 0;
}