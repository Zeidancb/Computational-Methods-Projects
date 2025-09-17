#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/AbsFunction.h"
#include "QatGenericFunctions/F1D.h"
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Exp.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
int main (int argc, char * * argv) {

  using namespace Genfun;

  std::cout << "\nSelect a function to plot:\n"
  << "  1) f(x) = sin(x)\n"
  << "  2) f(x) = sin(5x)\n"
  << "  3) f(x) = sin(x^2)\n"
  << "  4) f(x) = e^{-x} * sin(x)\n"
  << "  5) f(x) = P^2_3(x) = 15 x (1 - x^2)\n"
  << "  6) f(x) = tan(x)\n"
  << "  7) f'(x) where f(x) = tan(x)  (i.e., sec^2 x)\n"
  << "Enter 1-7: " << std::flush;

  int choice = 0;
  if (!(std::cin >> choice) || choice < 1 || choice > 7) {
    std::cerr << "Invalid choice; defaulting to 4 (e^{-x} sin x).\n";
    // clean bad input state if any
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    choice = 4;
  }

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
  rect.setXmin(-5.0);
  rect.setXmax(5.0);
  rect.setYmin(-5.0);
  rect.setYmax(5.0);
  

  PlotView view(rect);
  window.setCentralWidget(&view);

  Variable X;
  GENFUNCTION f1 = Sin();
  GENFUNCTION f2 = Sin()(5*X);
  GENFUNCTION f3 = Sin()(X*X);
  GENFUNCTION f4 = Exp()(-X) * Sin()(X);
  GENFUNCTION f5 = 15*X*(1 - X*X);
  GENFUNCTION f6 = F1D(tan);
  GENFUNCTION f7 = f6.prime();

  const AbsFunction *selected = &f1; // default
  std::string titleText = "f(x) = sin(x)";

  switch (choice) {
    case 1:
      selected  = &f1;
      titleText = "f(x) = sin(x)";
      break;
    case 2:
      selected  = &f2;
      titleText = "f(x) = sin(5x)";
      break;
    case 3:
      selected  = &f3;
      titleText = "f(x) = sin(x^2)";
      break;
    case 4:
      selected  = &f4;
      titleText = "f(x) = e^{-x} sin(x)";
      break;
    case 5:
      selected  = &f5;
      titleText = "f(x) = P^2_3(x) = 15 x (1 - x^2)";
      break;
    case 6:
      selected  = &f6;
      titleText = "f(x) = tan(x)";
      break;
    case 7:
      selected  = &f7;
      titleText = "f(x) = (tan x)' = sec^2(x)";
      break;
  }


  PlotFunction1D P(*selected);
  view.add(&P);
  
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

