#include <complex>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <numbers>
// 
// program that calculates the wave function incident to a delta spike potential 
// and returns the proportion reflected and transmitted through the potential
//
int main () {

    double m = 1.0;
    double hbar = 1.0;
    double E = 1.0;

    double start = 0.0;
    double step = 0.05;
    int N = 22;

    for (int i = 0; i < N; ++i) {
        // for loop sets the ratio between 0 and 2 for V/E as defined
        //in the problem statement
        double V = start + i * step;
        
        std::complex<double> dif = E - V;
        std::complex<double> k1 = std::sqrt(2 * m * E) / hbar;
        std::complex<double> k2 = std::sqrt(2 * m * dif) / hbar;
        
        //Define variables to make the code easier on the eyes
        std::complex<double> a = k1 + k2;
        std::complex<double> b = k1 - k2;
        std::complex<double> c = 2.0 * k1;

        std::complex<double>  r = b / a;
        std::complex<double>  t = c / a;

        double R = std::norm(r);
        double T = (std::real(k2) / std::real(k1)) * std::norm(t);

        //doesnt work, need ratio outside of norm
        //double T = (std::abs(c) * std::abs(c))/(std::abs(a) * std::abs(a));
        //double R = (std::abs(b) * std::abs(b))/(std::abs(a) * std::abs(a));

        //Prints values for T and R, sum, and full wave function using stepfunctions to combine the two partials
        std::cout << "V/E = " << std::fixed << V/E
        << "   T = " << std::setprecision(6) << T
        << "   R = " << R
        << "   R+T = " << (T + R)
        << "   Wavefunction psi(x) = " << "(exp(i*" << k1 <<"*x) + (" << r << ")*exp(-i*" << k1 <<"*x))*Theta(-x) + " << "(" << t << ")*exp(i*" << k2 <<"*x)*Theta(x)"
        << std::endl;
    }

}