#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include "QatGenericFunctions/InterpolatingFunction.h"
#include <vector>
#include <cmath>
#include <iostream>
int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

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
  rect.setXmin(150.0);
  rect.setXmax(1050.0);
  rect.setYmin(1.3);
  rect.setYmax(1.4);
  

  PlotView view(rect);
  window.setCentralWidget(&view);

  std::vector<double> xs{200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000};
  std::vector<double> ys{1.396, 1.362, 1.349, 1.343, 1.339, 1.337, 1.335, 1.333, 1.332, 1.331, 1.331, 1.330, 1.329, 1.329, 1.328, 1.327, 1.327};
    
  Genfun::InterpolatingFunction poly{Genfun::CUBIC_SPLINE};
  for (std::size_t i=0;i<xs.size();++i) poly.addPoint(xs[i], ys[i]);
  PlotFunction1D P_lag(poly);
  view.add(&P_lag);

  
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

