#include "SineIntegral.h"

#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Cos.h"

#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotKey.h"

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QPen>
#include <QColor>

int main (int argc, char **argv) {

  QApplication app(argc, argv);

  QMainWindow window;
  QToolBar *toolBar = window.addToolBar("Tools");
  QAction  *nextAction = toolBar->addAction("Next");
  nextAction->setShortcut(QKeySequence("n"));
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));

  PRectF rect;
  rect.setXmin(-20.0);
  rect.setXmax( 20.0);
  rect.setYmin(-2.0);
  rect.setYmax(  2.0);

  PlotView view(rect);
  view.setXZero(true);
  view.setYZero(true);
  view.setGrid(true);
  view.setFixedWidth(900);
  view.setFixedHeight(700);
  window.setCentralWidget(&view);


  SineIntegral si;
  Genfun::GENFUNCTION F = si; // + Genfun::Cos()
  Genfun::GENFUNCTION Fprime = F.prime(); 

  
  PlotFunction1D P_Si(F);
  PlotFunction1D P_SiPrime(Fprime);

  {
    PlotFunction1D::Properties prop;
    prop.pen = QPen(Qt::blue);
    P_Si.setProperties(prop);
  }
  {
    PlotFunction1D::Properties prop;
    prop.pen = QPen(Qt::red);
    P_SiPrime.setProperties(prop);
  }

  view.add(&P_Si);
  view.add(&P_SiPrime);

  PlotKey *key = new PlotKey(-19.0, 1.8);
  key->add(&P_Si,      "Si(x)");
  key->add(&P_SiPrime, "Si'(x)");
  view.add(key);

  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
              << PlotStream::Center()
              << PlotStream::Family("Sans Serif")
              << PlotStream::Size(16)
              << "Sine integral Si(x) and its derivative"
              << PlotStream::EndP();

  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(14)
               << "x"
               << PlotStream::EndP();

  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
               << PlotStream::Center()
               << PlotStream::Family("Sans Serif")
               << PlotStream::Size(14)
               << "f(x)"
               << PlotStream::EndP();

  view.show();
  window.show();

  app.exec();
  return 0;
}