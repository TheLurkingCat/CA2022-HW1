#pragma once
#include <Eigen/Core>

#include "buffer.h"
#include "shape.h"
#include "vertexarray.h"

class Spheres final : public Shape {
 public:
  static Spheres& initSpheres();
  void addSphere(const Eigen::Vector4f& position);
  void draw() const;
  void update(const Integrator& integrator);

 private:
  Spheres();

  int sphereCount;
  VertexArray vao;
  ArrayBuffer vbo;
  ArrayBuffer offsets;
  ElementArrayBuffer ebo;
};
