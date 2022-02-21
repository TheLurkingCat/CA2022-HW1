#pragma once
#include <Eigen/Core>
#include <vector>

#include "buffer.h"
#include "shape.h"
#include "vertexarray.h"

class Spheres final : public Shape {
 public:
  static Spheres& initSpheres();
  void addSphere(const Eigen::Vector4f& position, float size);
  void draw() const;
  void update(const Integrator& integrator) override;
  void collide(Shape* shape) override;
  void collide(Cloth* cloth) override;
  void collide() override;
  float radius(int i) const { return _radius[i]; }

 private:
  Spheres();

  int sphereCount;
  std::vector<float> _radius;
  VertexArray vao;
  ArrayBuffer vbo;
  ArrayBuffer offsets;
  ArrayBuffer sizes;
  ElementArrayBuffer ebo;
};
