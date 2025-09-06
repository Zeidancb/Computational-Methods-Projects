#include <complex>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <numbers>
// 
// threePi is a small program to obtain pi in one way
//
int main () {

  // 2. From the area of inner inscribed polygons.  Start
  //    with a hexagon and subdivide the arc into smaller
  //    pieces. Liu Hui(n=3072, 265 AD)  
  //    Zu Chongzhi (n=12288, 480 AD) 

  std::complex x0(1.0);
  std::complex x1(1.0/2.,sqrt(3.0)/2.0);
  unsigned int nsides=6;
  while (nsides < std::numeric_limits<int>::max()) {

    // Area can be written as A_t = 0.5 * | x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2) |
    // Note that the base of the isosceles triangle makes up a single side
    // of the polygon, and hence A = n * A_t where A_t denotes the area 
    // of the single triangle
    double area = 0.5 * std::abs(std::imag(std::conj(x0) * x1));
    double approx= nsides * area;
    std::cout << "Sides " << nsides << "; approx=" << std::setprecision(16) << approx << std::endl;

    x1=(x1+x0)/std::abs(x1+x0);
    nsides *=2; 
  }
}
