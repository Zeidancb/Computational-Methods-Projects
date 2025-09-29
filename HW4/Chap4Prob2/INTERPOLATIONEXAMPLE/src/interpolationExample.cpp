#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Sqrt.h"
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

int main (int argc, char * * argv) {


  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }


  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(-1.0);
  rect.setXmax( 1.0);
  rect.setYmin( 0.0);
  rect.setYmax( 1.0);
  

  PlotView view(rect);
  window.setCentralWidget(&view);

  // Insert plots
  using namespace Genfun;
  Variable X;

  GENFUNCTION exact = Sqrt()(1.0 - X*X);
  PlotFunction1D P_exact(exact);
  view.add(&P_exact);

  std::vector<double> xs, ys;
  const int N = 6;
  xs.reserve(N+1); ys.reserve(N+1);
  const double PI = std::acos(-1.0);

  for (int j = N; j >= 0; --j) {
    double theta = PI * j / N;
    double x = std::cos(theta);
    double y = std::sin(theta);
    xs.push_back(x);
    ys.push_back(y);
  }

  Genfun::InterpolatingFunction lag; 
  for (std::size_t i = 0; i < xs.size(); ++i) lag.addPoint(xs[i], ys[i]);
  PlotFunction1D P_lag(lag);
  view.add(&P_lag);

  Genfun::InterpolatingFunction spl{CUBIC_SPLINE};
  for (std::size_t i = 0; i < xs.size(); ++i) spl.addPoint(xs[i], ys[i]);
  PlotFunction1D P_spl(spl);
  view.add(&P_spl);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Arial") 
	      << PlotStream::Size(16)
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << PlotStream::EndP();
  
  
  
  window.show();
  app.exec();
  return 1;
}