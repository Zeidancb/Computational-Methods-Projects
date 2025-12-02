#include "QatGenericFunctions/LegendrePolynomial.h"
#include "QatGenericFunctions/Variable.h"

#include "QatDataAnalysis/Hist1D.h"

#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotKey.h"

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>

#include <random>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace Genfun;

int main (int argc, char **argv) {

  // events
  std::size_t N = (argc > 1) ? std::strtoull(argv[1], nullptr, 10) : 100000;

  // rng setup
  std::random_device rd;
  std::mt19937_64 rng(rd());
  std::uniform_real_distribution<double> distX(-1.0, 1.0);
  std::uniform_real_distribution<double> distY(0.0, 2.0); // m=2 above max rho

  // legendre rho
  LegendrePolynomial P1(1);
  LegendrePolynomial P3(3);
  LegendrePolynomial P5(5);
  LegendrePolynomial P7(7);

  GENFUNCTION A  = std::sqrt(3.0/8.0)*P1
                 - std::sqrt(7.0/6.0)*P3
                 + std::sqrt(11.0/24.0)*P5
                 - std::sqrt(15.0/6.0)*P7;

  GENFUNCTION rho = A*A;   // pdf on [-1,1]

  // cos(theta) hist
  const int    nBins = 80;
  const double xMin  = -1.0;
  const double xMax  =  1.0;
  const double binW  = (xMax - xMin)/nBins;

  Hist1D hist("cos(theta)", nBins, xMin, xMax);

  std::size_t accepted = 0;
  while (accepted < N) {
    double x = distX(rng);
    double y = distY(rng);

    if (y < rho(x)) {       // accept-reject
      hist.accumulate(x);
      ++accepted;
    }
  }

  // scale rho
  double scale = accepted * binW;
  GENFUNCTION rhoScaled = scale * rho;

  // ymax guess
  double yMax = 2.0 * scale;

  QApplication app(argc, argv);

  QMainWindow window;
  QToolBar *toolBar = window.addToolBar("Tools");
  QAction  *nextAction = toolBar->addAction("Next");
  nextAction->setShortcut(QKeySequence("n"));
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));

  PRectF rect;
  rect.setXmin(xMin);
  rect.setXmax(xMax);
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

  // rho curve
  PlotFunction1D P_rho(rhoScaled);
  {
    PlotFunction1D::Properties prop;
    prop.pen.setColor(Qt::red);
    prop.pen.setWidth(2);
    P_rho.setProperties(prop);
  }
  view.add(&P_rho);

  // plot legend
  PlotKey *key = new PlotKey(-0.95, 0.9 * yMax);
  key->add(&P_hist, "mc histogram (counts)");
  key->add(&P_rho,  "analytic ρ(cosθ) × nΔx");
  view.add(key);

  // title/labels
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
              << PlotStream::Center()
              << PlotStream::Family("Sans Serif")
              << PlotStream::Size(16)
              << "angular distribution ρ(cosθ)"
              << PlotStream::EndP();

  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(14)
               << "cos θ"
               << PlotStream::EndP();

  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(14)
               << "counts per bin"
               << PlotStream::EndP();

  view.show();
  window.show();

  app.exec();
  return 0;
}