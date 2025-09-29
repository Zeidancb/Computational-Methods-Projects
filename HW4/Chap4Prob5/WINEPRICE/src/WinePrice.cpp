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


  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(2005);
  rect.setXmax(2028);
  rect.setYmin(300);
  rect.setYmax(220000);
  

  PlotView view(rect);
  window.setCentralWidget(&view);

  std::vector<double> xs{2007,2008,2009,2010,2011};
  std::vector<double> ys{ 346, 654,724,873,1410};
    
  Genfun::InterpolatingFunction poly{Genfun::POLYNOMIAL};
  for (std::size_t i=0;i<xs.size();++i) poly.addPoint(xs[i], ys[i]);
  PlotFunction1D P_lag(poly);
  view.add(&P_lag);

  double year = 2025;                 
  double price = poly(year);
  double profit = poly(year + 1);              
  std::cout << year << "  --->  " << price << "  Profit after 1 Year:  " << profit - price;
  
    // Automatically generated:-------------------------:
  
    std::string usage= std::string("usage: ") + argv[0]; 
    if (argc!=1) {
      std::cout << usage << std::endl;
    }
  
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

