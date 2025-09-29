#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/AbsFunction.h"
#include "QatGenericFunctions/F1D.h"
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Cos.h"
#include "QatGenericFunctions/Exp.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <Eigen/Dense>
#include <sstream>
#include <complex>
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Cos.h"
#include "QatGenericFunctions/Exp.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotStream.h"
#include "QatGenericFunctions/AbsFunction.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Cos.h"
#include "QatGenericFunctions/Exp.h"
#include <algorithm>   // for std::max
int main (int argc, char * * argv) {

  using namespace std;
  string usage = string("Usage: ") 
    + argv[0] 
    + " [-?] [-v potential] [-k wvnumber]";
   
  if (argc>1 && argv[1]==string("-?")) {
    cout << usage << endl;
    return 1;
  }

  // default values
   double v=0.5;
   double k=0.2;
 
  try {
    // overwritten by command line:
    for (int i=1; i<argc;i+=2) {
      istringstream stream(argv[i+1]);
      if (string(argv[i])=="-v") stream >> v;
      if (string(argv[i])=="-k") stream >> k;
    }
  }
  catch (exception &) {
    cout << usage << endl;
    return 1;
  }

  complex I(0.0,1.0);
  complex nk=k*sqrt(complex(1.0-v)),Ik=I*k,Ink=I*nk;

  Eigen::VectorXcd Y{{-exp(-Ik)    },
		     {0.0           },
		     {-Ik*exp(-Ik)},
		     {0.0}          };

  Eigen::MatrixXcd A{{exp(Ik),     -exp(-Ink),   -exp(Ink),       0.0        },
		     {0       ,    exp(Ink),      exp(-Ink),     -exp(Ik)    },
		     {-Ik*exp(Ik),-Ink*exp(-Ink), Ink*exp(Ink),   0          },
		     {0          , Ink*exp(Ink), -Ink*exp(-Ink), -Ik*exp(Ik)}};

  Eigen::MatrixXcd AInv= A.inverse();
  Eigen::VectorXcd BCDF=AInv*Y;
  complex B=BCDF(0), F=BCDF(3);


  using namespace Genfun;

  Variable X;
  Exp      expf;
  Cos      cosf;
  
  double r      = std::abs(B);
  double phi_r  = std::arg(B);
  double Tconst = std::norm(F);
  std::complex<double> C = BCDF(1), D = BCDF(2);
  double a      = std::real(nk);
  double b      = std::imag(nk);
  double cabs   = std::abs(C);
  double dabs   = std::abs(D);
  double phi_cd = std::arg(C*std::conj(D));
  
  // Functions for each region
  GENFUNCTION rhoI   = 1 + r*r + 2*r * cosf(2*k*X + phi_r);
  GENFUNCTION rhoII  = cabs*cabs * expf(-2*b*X)
                     + dabs*dabs * expf( 2*b*X)
                     + 2*cabs*dabs * cosf(2*a*X + phi_cd);
  GENFUNCTION rhoIII = Tconst + 0*X;
  
  const double L = 1.0;
  const double s = 40.0;

  GENFUNCTION H_left  = 1.0 / (1.0 + expf(-s * (-(X+L))));
  GENFUNCTION H_midL  = 1.0 / (1.0 + expf(-s * ( X+L)));
  GENFUNCTION H_midR  = 1.0 / (1.0 + expf(-s * ( L-X)));
  GENFUNCTION H_right = 1.0 / (1.0 + expf(-s * ( X-L)));
  GENFUNCTION Gleft  = H_left;           // x <= -L
  GENFUNCTION Gmid   = H_midL * H_midR;  // -L <= x <= L
  GENFUNCTION Gright = H_right;          // x >=  L
  
  GENFUNCTION rho = rhoI*Gleft + rhoII*Gmid + rhoIII*Gright;
  
  QApplication     app(argc,argv);
  QMainWindow      window;
  PRectF rect;
  rect.setXmin(-2.0); rect.setXmax(2.0);
  rect.setYmin(0.0);  rect.setYmax(std::max(2.0, 1 + 2*r + r*r));
  PlotView view(rect);
  window.setCentralWidget(&view);
  
  PlotFunction1D Prho(rho);
  view.add(&Prho);

  
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