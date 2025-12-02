#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotKey.h"

#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Exp.h"

#include "QatDataAnalysis/Hist1D.h"

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>

#include <random>
#include <cmath>
#include <iostream>
#include <cstdlib>

using namespace Genfun;

int main (int argc, char **argv) {

  // sample count
  std::size_t Nsamples = (argc > 1)
                         ? std::strtoull(argv[1], nullptr, 10)
                         : 100000;

  // u range
  const int    nBins = 80;
  const double uMin  = 0.0;
  const double uMax  = 4.0;
  const double binW  = (uMax - uMin)/nBins;

  // rng and gamma
  std::random_device rd;
  std::mt19937_64 rng(rd());
  std::gamma_distribution<double> gammaDist(1.5, 1.0);  // k=3/2, θ=1

  // histogram u
  Hist1D hist("u = v/v0", nBins, uMin, uMax);

  for (std::size_t i = 0; i < Nsamples; ++i) {
    double x = gammaDist(rng);    // gamma sample
    double u = std::sqrt(x);      // dimensionless speed
    hist.accumulate(u);
  }

  // maxwell pdf
  const double PI = 3.14159265358979323846;
  Variable u;
  Exp exp;

  GENFUNCTION rhoPDF = (4.0/std::sqrt(PI)) * u*u * exp(-u*u);

  // scale to counts
  double scale = double(Nsamples) * binW;
  GENFUNCTION rhoScaled = scale * rhoPDF;

  // y axis guess
  double yMax = 1.2 * scale;

  // qat
  QApplication app(argc, argv);

  QMainWindow window;
  QToolBar *toolBar = window.addToolBar("Tools");
  QAction  *quitAction = toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));

  PRectF rect;
  rect.setXmin(uMin);
  rect.setXmax(uMax);
  rect.setYmin(0.0);
  rect.setYmax(yMax);

  PlotView view(rect);
  view.setXZero(false);
  view.setYZero(false);
  view.setGrid(true);
  view.setFixedWidth(900);
  view.setFixedHeight(700);
  window.setCentralWidget(&view);

  // hist plot
  PlotHist1D P_hist(hist);
  {
    PlotHist1D::Properties prop;
    prop.pen.setColor(Qt::darkGray);
    prop.pen.setWidth(1);
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor(QColor(200,200,200));
    P_hist.setProperties(prop);
  }
  view.add(&P_hist);

  // analytic curve
  PlotFunction1D P_rho(rhoScaled);
  {
    PlotFunction1D::Properties prop;
    prop.pen.setColor(Qt::red);
    prop.pen.setWidth(2);
    P_rho.setProperties(prop);
  }
  view.add(&P_rho);

  // title
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
              << PlotStream::Center()
              << PlotStream::Family("Sans Serif")
              << PlotStream::Size(16)
              << "maxwell speed distribution via gamma sampling"
              << PlotStream::EndP();

  // x label
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(14)
               << "u = v / v₀   (dimensionless speed)"
               << PlotStream::EndP();

  // y label
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(14)
               << "counts per bin"
               << PlotStream::EndP();

  window.show();
  app.exec();

  return 0;
}