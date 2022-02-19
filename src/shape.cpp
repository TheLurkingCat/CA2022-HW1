#include "shape.h"

#include <Eigen/Dense>

using Eigen::Matrix4f;

Shape::Shape(int size) noexcept :
    particles(size), modelMatrix(Matrix4f::Identity()), normalMatrix(Matrix4f::Identity()) {}

void Shape::setModelMatrix(const Eigen::Matrix4f& _modelMatrix) {
  modelMatrix = _modelMatrix;
  normalMatrix.topLeftCorner<3, 3>() = _modelMatrix.topLeftCorner<3, 3>().inverse().transpose();
}
