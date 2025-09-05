#include <complex>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <numbers>
// 
// Program that tests multiple different trig identities 
//between normal trig functions and hyperbolic trig functions
//
bool test1(std::complex<double> x) {
    
    std::complex<double> i(0.0, 1.0);

    std::complex<double> testSin = sin(i * x);
    std::complex<double> testSinh = i * sinh(x);
    return testSin == testSinh;
}

bool test2(std::complex<double> x) {
    
    std::complex<double> i(0.0, 1.0);

    std::complex<double> testCos = cos(i * x);
    std::complex<double> testCosh = cosh(x);
    return testCos == testCosh;
}

int main () {

    std::complex<double> i(0.0, 1.0);

    std::complex<double> test1(0.0, 0.0);
    std::complex<double> test2(1.0, 0.0);
    std::complex<double> test3(0.0, 1.0);
    std::complex<double> test4(M_PI, M_PI);
    std::complex<double> test5(M_PI_2, 0.0);
    std::complex<double> test6(0.0, M_PI_2);
    std::complex<double> test7(M_PI_2, M_PI_2);


}