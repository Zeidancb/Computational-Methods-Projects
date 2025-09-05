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
bool testSinId(std::complex<double> x) {
    
    std::complex<double> i(0.0, 1.0);

    std::complex<double> testSin = sin(i * x);
    std::complex<double> testSinh = i * sinh(x);
    return testSin == testSinh;
}

bool testCosId(std::complex<double> x) {
    
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

    std::cout << "x = " << test1
    << "  test1: " << (testSinId(test1) ? "true" : "false")
    << "  test2: " << (testCosId(test1) ? "true" : "false")
    << std::endl;

    std::cout << "x = " << test2
    << "  test1: " << (testSinId(test2) ? "true" : "false")
    << "  test2: " << (testCosId(test2) ? "true" : "false")
    << std::endl;

    std::cout << "x = " << test3
    << "  test1: " << (testSinId(test3) ? "true" : "false")
    << "  test2: " << (testCosId(test3) ? "true" : "false")
    << std::endl;

    std::cout << "x = " << test4
    << "  test1: " << (testSinId(test4) ? "true" : "false")
    << "  test2: " << (testCosId(test4) ? "true" : "false")
    << std::endl;
}