#pragma once
#include <glad/gl.h>
#include <vector>

#include "buffer.h"
#include "shape.h"
#include "spring.h"
#include "utils.h"
#include "vertexarray.h"

class Cloth final : public Shape {
 public:
  MOVE_ONLY(Cloth)
  enum class DrawType { FULL, STRUCTURAL, SHEAR, BEND, PARTICLE };
  Cloth();
  std::vector<Spring>& springs() { return _springs; }
  void draw(DrawType type) const;
  void computeSpringForce();
  void computeNormal();
  void collide(Shape* shape) override;
  void collide(Spheres* sphere) override;

 private:
  void initializeVertex();
  void initializeSpring();
  std::vector<Spring> _springs;
  VertexArray vao;
  ArrayBuffer positionBuffer;
  ArrayBuffer normalBuffer;
  ElementArrayBuffer ebo, structuralSpring, shearSpring, bendSpring;
};
