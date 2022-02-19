#pragma once
#include <Eigen/Core>
#include "particles.h"

class Integrator;

class Shape {
 public:
  explicit Shape(int size) noexcept;
  virtual ~Shape() = default;

  void setModelMatrix(const Eigen::Matrix4f& _modelMatrix);
  Eigen::Matrix4f getModelMatrix() const { return modelMatrix; }
  Eigen::Matrix4f getNormalMatrix() const { return normalMatrix; }
  virtual void update(const Integrator& integrator) = 0;

 protected:
  Particles particles;
  Eigen::Matrix4f modelMatrix;
  Eigen::Matrix4f normalMatrix;
};
