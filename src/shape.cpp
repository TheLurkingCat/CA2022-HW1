#include "shape.h"

#include <Eigen/Dense>
#include "integrator.h"

using Eigen::Matrix4f;

Shape::Shape(int size, float mass_) noexcept :
    _particles(size, mass_), modelMatrix(Matrix4f::Identity()), normalMatrix(Matrix4f::Identity()) {}

void Shape::setModelMatrix(const Eigen::Ref<const Eigen::Matrix4f>& _modelMatrix) {
  modelMatrix = _modelMatrix;
  normalMatrix.topLeftCorner<3, 3>() = _modelMatrix.topLeftCorner<3, 3>().inverse().transpose();
}

void Shape::resetAcceleration() { _particles.acceleration().colwise() = Eigen::Vector4f(0, -9.8f, 0, 0); }

void Shape::update(const Integrator* integrator, std::function<void(void)> simulateOneStep) {
  integrator->integrate(_particles, simulateOneStep);
}
