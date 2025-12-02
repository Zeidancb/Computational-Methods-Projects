#include <iostream>
#include <cmath>
#include <random>
#include <vector>
#include <memory>
#include <iomanip>

// 3D vec
struct Vec3 {
  double x, y, z;
};

// base shape
class Shape3D {
public:
  virtual ~Shape3D() {}
  virtual bool contains(const Vec3 &p) const = 0;
  virtual double analyticVolume() const = 0;
  virtual const char *name() const = 0;
};

// sphere
class Sphere : public Shape3D {
public:
  Sphere(double radius) : R(radius) {}

  virtual bool contains(const Vec3 &p) const override {
    double r2 = p.x*p.x + p.y*p.y + p.z*p.z;
    return r2 <= R*R;
  }

  virtual double analyticVolume() const override {
    return (4.0/3.0) * M_PI * R*R*R;
  }

  virtual const char *name() const override { return "Sphere"; }

private:
  double R;
};

// cube, centered
class Cube : public Shape3D {
public:
  Cube(double side) : a(side) {}

  virtual bool contains(const Vec3 &p) const override {
    double half = 0.5 * a;
    return (std::fabs(p.x) <= half &&
            std::fabs(p.y) <= half &&
            std::fabs(p.z) <= half);
  }

  virtual double analyticVolume() const override {
    return a*a*a;
  }

  virtual const char *name() const override { return "Cube"; }

private:
  double a;
};

// cylinder along z
class Cylinder : public Shape3D {
public:
  Cylinder(double radius, double height) : R(radius), H(height) {}

  virtual bool contains(const Vec3 &p) const override {
    double r2 = p.x*p.x + p.y*p.y;
    return (r2 <= R*R) && (std::fabs(p.z) <= H*0.5);
  }

  virtual double analyticVolume() const override {
    return M_PI * R*R * H;
  }

  virtual const char *name() const override { return "Cylinder"; }

private:
  double R, H;
};

// simple tetra in first octant
class Tetrahedron : public Shape3D {
public:
  Tetrahedron(double edge) : a(edge) {}

  virtual bool contains(const Vec3 &p) const override {
    if (p.x < 0.0 || p.y < 0.0 || p.z < 0.0) return false;
    if (p.x + p.y + p.z > a) return false;
    return true;
  }

  virtual double analyticVolume() const override {
    return a*a*a / 6.0;
  }

  virtual const char *name() const override { return "Tetrahedron"; }

private:
  double a;
};

// MC volume
double monteCarloVolume(const Shape3D &shape,
                        const Vec3 &minCorner,
                        const Vec3 &maxCorner,
                        std::size_t N,
                        std::mt19937_64 &rng) {
  std::uniform_real_distribution<double> distX(minCorner.x, maxCorner.x);
  std::uniform_real_distribution<double> distY(minCorner.y, maxCorner.y);
  std::uniform_real_distribution<double> distZ(minCorner.z, maxCorner.z);

  std::size_t inside = 0;
  for (std::size_t i = 0; i < N; ++i) {
    Vec3 p{distX(rng), distY(rng), distZ(rng)};
    if (shape.contains(p)) ++inside;
  }

  double Vbox =
      (maxCorner.x - minCorner.x) *
      (maxCorner.y - minCorner.y) *
      (maxCorner.z - minCorner.z);

  double fraction = static_cast<double>(inside) / static_cast<double>(N);
  return Vbox * fraction;
}

// main
int main() {
  std::random_device rd;
  std::mt19937_64 rng(rd());

  Sphere       sphere(1.0);
  Cube         cube(2.0);
  Cylinder     cylinder(1.0, 2.0);
  Tetrahedron  tetra(1.0);

  std::vector<Shape3D*> shapes = { &sphere, &cube, &cylinder, &tetra };

  std::size_t N = 500000;

  std::cout << std::fixed << std::setprecision(6);

  for (Shape3D* s : shapes) {
    const char *nm = s->name();
    double analytic = s->analyticVolume();

    Vec3 minC, maxC;
    if (nm == std::string("Tetrahedron")) {
      minC = {0.0, 0.0, 0.0};
      maxC = {1.0, 1.0, 1.0};
    } else {
      minC = {-1.0, -1.0, -1.0};
      maxC = { 1.0,  1.0,  1.0};
    }
            
    double estimate = monteCarloVolume(*s, minC, maxC, N, rng);

    std::cout << nm
              << ":\n  Analytic volume = " << analytic
              << "\n  MC estimate      = " << estimate
              << "\n  Relative error   = "
              << std::fabs(estimate - analytic) / analytic
              << "\n\n";
  }

  return 0;
}