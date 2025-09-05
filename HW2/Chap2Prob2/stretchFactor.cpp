#include <complex>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <numbers>
// 
// stretchFactor is a project that calculates the stretch factor
// \gamma  for relativistic particles in two different ways.
//
int main () {
    int limit = 100;
    for (int n = 1; n <= limit; ++n) {
        
        //Defines both beta and gamma
        double beta = 1.0 - std::pow(10.0, -n);
        double epsilon = std::pow(10.0, -n);
        
        //Calculates gamma both ways
        double gamma1 = 1 / std::sqrt(1 - (beta * beta));
        double gamma2 = 1 / std::sqrt((2 - epsilon) * epsilon);
        double fracError = std::fabs(gamma1 - gamma2) / std::fabs(gamma2);

        //Prints the value of beta, stretch factor value, and the fractional error when comparing the two methods
        std::cout << "n = " << n << "  beta = " << std::setprecision(10) << beta << "  gamma1 = " << gamma1 << "  gamma2 = " << gamma2 << "  fracError = " << fracError << std::endl;

        //Applying this code, I found that the highest value of beta was at n = 15, and the fraction error limit was violated at beta = 1.0 - 10.0^(-16)
        if(fracError > 0.001){
            n = 101;
        }
    }
}