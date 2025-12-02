#ifndef SINEINTEGRAL_H
#define SINEINTEGRAL_H

#include "QatGenericFunctions/AbsFunction.h"
#include "QatGenericFunctions/Argument.h"

#include <gsl/gsl_sf.h>
#include <stdexcept>

// sine integral wrapper
class SineIntegral: public Genfun::AbsFunction {
public:
  SineIntegral() {}
  SineIntegral(const SineIntegral & right):
    Genfun::AbsFunction(right) {}
  virtual ~SineIntegral() {}

  // 1D
  virtual unsigned int dimensionality() const {
    return 1;
  }

  // f(x)
  virtual double operator()(double x) const {
    return gsl_sf_Si(x);
  }

  // f(arg)
  virtual double operator()(const Genfun::Argument & a) const {
    if (a.dimension() != 1)
      throw std::range_error("SineIntegral: Argument must be 1D");
    return (*this)(a[0]);
  }

  // clone
  virtual SineIntegral * clone() const {
    return new SineIntegral(*this);
  }

private:
  // no assign
  SineIntegral & operator=(const SineIntegral &);
};

#endif