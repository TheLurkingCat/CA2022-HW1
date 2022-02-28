#pragma once
#include <glad/gl.h>
#include <vector>

#include "buffer.h"
#include "shape.h"
#include "spring.h"
#include "vertexarray.h"

class Cloth final : public Shape {
 public:
  enum class DrawType { FULL, STRUCTURAL, SHEAR, BEND, PARTICLE };
  Cloth();
  void draw(DrawType type) const;
  void computeSpringForce();
  void collide(Shape* shape) override;
  void collide(Spheres* sphere) override;

 private:
  void initializeVertex();
  void initializeSpring();
  std::vector<Spring> springs;
  VertexArray vao;
  ArrayBuffer positionBuffer;
  ArrayBuffer texCoordsBuffer;
  ElementArrayBuffer ebo, structuralSpring, shearSpring, bendSpring;
};
