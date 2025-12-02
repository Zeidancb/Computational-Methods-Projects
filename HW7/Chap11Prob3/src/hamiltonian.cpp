#include "QatGenericFunctions/RungeKuttaClassicalSolver.h"
#include "QatGenericFunctions/PhaseSpace.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Cos.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <QApplication>

#include "orbit.h"
#include "plot.h"

using namespace Genfun;

int main (int argc, char **argv) {

  // basic params
  double m = 1.0;
  double g = 1.0;
  double a = 1.0;

  // scale stuff
  double E      = m * g * a;
  double lambda = m * std::pow(a, 1.5) * std::sqrt(g);

  // p0 = l0 / lambda
  std::vector<double> p0List = {
    0.25,
    0.5,
    1.0,
    std::sqrt(2.0),
    2.0,
    3.0
  };

  QApplication app(argc, argv);

  // loop over initial momenta
  for (std::size_t i = 0; i < p0List.size(); ++i) {

    double p0 = p0List[i];
    double l0 = p0 * lambda;

    Classical::PhaseSpace phaseSpace(1);
    const Classical::PhaseSpace::Component
      & q = phaseSpace.coordinates(),
      & p = phaseSpace.momenta();

    GENFUNCTION H =
      (p[0]*p[0])/(2.0 * m * a * a)
      - (m * g * a) * Cos()(q[0]);

    // theta(0)=0, l(0)=l0
    phaseSpace.start(q[0], 0.0);
    phaseSpace.start(p[0], l0);

    Classical::RungeKuttaSolver solver(H, phaseSpace);

    // plot orbit: theta vs l/lambda
    orbit(app,
          solver.equationOf(q[0]),
          solver.equationOf(p[0]) / lambda);
  }

  return 0;
}