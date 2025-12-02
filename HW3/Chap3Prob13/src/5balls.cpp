// five masses
// eigen + qat

#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/InterpolatingFunction.h"

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>

#include <Eigen/Dense>
#include <cmath>
#include <iostream>

using namespace Eigen;
using namespace Genfun;

int main(int argc, char **argv) {

  // mass and k
  const int N = 5;

  // mass matrix
  MatrixXd M = MatrixXd::Zero(N, N);
  M(0,0) = 1.0;
  M(1,1) = 3.0;
  M(2,2) = 2.0;
  M(3,3) = 1.0;
  M(4,4) = 2.0;

  // stiffness matrix
  MatrixXd K(N, N);
  K <<  1.0, -1.0,  0.0,  0.0,  0.0,
       -1.0,  3.0, -2.0,  0.0,  0.0,
        0.0, -2.0,  4.0, -2.0,  0.0,
        0.0,  0.0, -2.0,  3.0, -1.0,
        0.0,  0.0,  0.0, -1.0,  1.0;

  VectorXd masses(N);
  masses << 1.0, 3.0, 2.0, 1.0, 2.0;
  double totalMass = masses.sum();

  VectorXd invSqrtM(N);
  invSqrtM(0) = 1.0/std::sqrt(1.0);
  invSqrtM(1) = 1.0/std::sqrt(3.0);
  invSqrtM(2) = 1.0/std::sqrt(2.0);
  invSqrtM(3) = 1.0/std::sqrt(1.0);
  invSqrtM(4) = 1.0/std::sqrt(2.0);

  // m^{-1/2} and b
  MatrixXd MinvSqrt = invSqrtM.asDiagonal();
  MatrixXd B        = MinvSqrt * K * MinvSqrt;

  SelfAdjointEigenSolver<MatrixXd> es(B);
  if (es.info() != Success) {
    std::cerr << "Eigen decomposition failed.\n";
    return 1;
  }

  VectorXd lambda = es.eigenvalues();
  MatrixXd V      = es.eigenvectors();

  VectorXd omega(N);
  for (int j = 0; j < N; ++j) {
    omega(j) = std::sqrt(std::max(0.0, lambda(j)));
  }
  MatrixXd PHI = MinvSqrt * V;

  // initial
  VectorXd x0(N);
  x0 << -2.0,  3.0,  0.0, -3.0,  2.0;

  VectorXd v0 = VectorXd::Zero(N);

  // modal amplitudes
  VectorXd c = PHI.transpose() * M * x0;

  // time
  const double tauMax = 30.0;
  const int    Nsteps = 1500;
  const double dtau   = tauMax / (Nsteps - 1);

  // interp
  InterpolatingFunction x1Func(CUBIC_SPLINE);
  InterpolatingFunction x2Func(CUBIC_SPLINE);
  InterpolatingFunction x3Func(CUBIC_SPLINE);
  InterpolatingFunction x4Func(CUBIC_SPLINE);
  InterpolatingFunction x5Func(CUBIC_SPLINE);
  InterpolatingFunction xCMFunc(CUBIC_SPLINE);

  double yMin =  1e9;
  double yMax = -1e9;

  for (int n = 0; n < Nsteps; ++n) {
    double tau = n * dtau;

    // cos
    VectorXd cosVec(N);
    for (int j = 0; j < N; ++j) {
      cosVec(j) = std::cos(omega(j) * tau);
    }

    VectorXd q = c.array() * cosVec.array();

    VectorXd x = PHI * q;

    // com
    double xCM = (masses.array() * x.array()).sum() / totalMass;

    // add points
    x1Func.addPoint(tau, x(0));
    x2Func.addPoint(tau, x(1));
    x3Func.addPoint(tau, x(2));
    x4Func.addPoint(tau, x(3));
    x5Func.addPoint(tau, x(4));
    xCMFunc.addPoint(tau, xCM);

    // track range
    for (int i = 0; i < N; ++i) {
      yMin = std::min(yMin, x(i));
      yMax = std::max(yMax, x(i));
    }
    yMin = std::min(yMin, xCM);
    yMax = std::max(yMax, xCM);
  }

  double margin = 0.5 * (yMax - yMin + 1e-6);
  if (margin <= 0.0) margin = 1.0;
  double Ymin = yMin - 0.1 * margin;
  double Ymax = yMax + 0.1 * margin;

  // qat plotting
  QApplication app(argc, argv);

  QMainWindow window;
  QToolBar *toolBar = window.addToolBar("Tools");
  QAction  *quitAction = toolBar->addAction("Quit");
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));

  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(tauMax);
  rect.setYmin(Ymin);
  rect.setYmax(Ymax);

  PlotView view(rect);
  view.setBox(true);
  view.setGrid(true);
  view.setXZero(false);
  view.setYZero(false);
  view.setFixedWidth(900);
  view.setFixedHeight(600);
  window.setCentralWidget(&view);

  PlotFunction1D Px1(x1Func);
  PlotFunction1D Px2(x2Func);
  PlotFunction1D Px3(x3Func);
  PlotFunction1D Px4(x4Func);
  PlotFunction1D Px5(x5Func);
  PlotFunction1D PxCM(xCMFunc);

  {
    PlotFunction1D::Properties p;
    p.pen.setWidth(2);

    p.pen.setColor(Qt::red);
    Px1.setProperties(p);

    p.pen.setColor(Qt::green);
    Px2.setProperties(p);

    p.pen.setColor(Qt::blue);
    Px3.setProperties(p);

    p.pen.setColor(Qt::magenta);
    Px4.setProperties(p);

    p.pen.setColor(Qt::darkCyan);
    Px5.setProperties(p);

    p.pen.setColor(Qt::black);
    p.pen.setStyle(Qt::DashLine);
    PxCM.setProperties(p);
  }

  // add plots
  view.add(&Px1);
  view.add(&Px2);
  view.add(&Px3);
  view.add(&Px4);
  view.add(&Px5);
  view.add(&PxCM);

  // title
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
              << PlotStream::Center()
              << PlotStream::Family("Sans Serif")
              << PlotStream::Size(16)
              << "five coupled masses on springs"
              << PlotStream::EndP();

  // x label
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(14)
               << "dimensionless time  \\\\tau = t \\\\sqrt{k/m}"
               << PlotStream::EndP();

  // y label
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(14)
               << "displacement (cm)"
               << PlotStream::EndP();

  view.show();
  window.show();

  return app.exec();
}