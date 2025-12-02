#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Theta.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Exp.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <libgen.h>
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
#include <algorithm>

#include "QatPlotting/PlotOrbit.h"
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Cos.h"
#include <QPen>

int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }


  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(631);
  rect.setYmin(0.0);
  rect.setYmax(442);

  PlotView view(rect);
  view.setXZero(false);
  view.setYZero(false);
  view.setGrid(false);
  view.setFixedWidth(900);
  view.setFixedHeight(700);
  window.setCentralWidget(&view);

  std::vector<double> xs{125, 235, 357, 383, 485, 514, 527, 525, 565};
  std::vector<double> ys{225, 305, 287, 297, 269, 240, 190, 110, 80};
    
  double x0=0.0, y0=0.0;
  for (std::size_t i=0;i<xs.size();++i){ x0+=xs[i]; y0+=ys[i]; }
  x0/=xs.size(); y0/=ys.size();

  std::vector<std::pair<double,double>> th_r;
  th_r.reserve(xs.size());
  for (std::size_t i=0;i<xs.size();++i){
    double th = std::atan2(ys[i]-y0, xs[i]-x0);
    double r  = std::hypot(xs[i]-x0, ys[i]-y0);
    th_r.push_back({th,r});
  }
  std::sort(th_r.begin(), th_r.end(),
            [](const auto &a, const auto &b){return a.first<b.first;});
  for (std::size_t i=1;i<th_r.size();++i){
    while (th_r[i].first<=th_r[i-1].first) th_r[i].first += 2*M_PI;
  }
  double th0 = th_r.front().first;
  double th1 = th0 + 2*M_PI;

  Genfun::InterpolatingFunction rOfTh(Genfun::CUBIC_SPLINE_PERIODIC);
  for (auto &p: th_r) rOfTh.addPoint(p.first, p.second);
  rOfTh.addPoint(th1, th_r.front().second);

  using namespace Genfun;
  Variable T;
  GENFUNCTION Xparam = x0 + rOfTh(T) * Cos()(T);
  GENFUNCTION Yparam = y0 + rOfTh(T) * Sin()(T);
  PlotOrbit orbit(Xparam, Yparam, th0, th1);
  PlotOrbit::Properties pr; pr.pen = QPen(Qt::red, 2);
  orbit.setProperties(pr);
  view.add(&orbit);

  char *wd=dirname(argv[0]);
  std::string imageName=std::string(wd)+"/../../src/p1.png";
  QPixmap imagePix(imageName.c_str());
  QGraphicsPixmapItem imagePixItem;
  imagePixItem.setPixmap(imagePix);
  //imagePixItem.setFlags(QGraphicsItem::ItemIsMovable);
  view.scene()->addItem(&imagePixItem);
  imagePixItem.setScale(1.0);
  imagePixItem.setPos(130,130);


  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "P1 Bus interpolated position"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "X position, pixel units"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "Y position, pixel units"
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}
