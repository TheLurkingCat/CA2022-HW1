#include "particles.h"

Particles::Particles(int size) noexcept :
    _position(4, size), _velocity(4, size), _acceleration(4, size), _mass(size, 0.0f) {
  _position.setZero();
  _velocity.setZero();
  _acceleration.setZero();
}
void Particles::resize(int newSize) {
  _position.resize(Eigen::NoChange, newSize);
  _velocity.resize(Eigen::NoChange, newSize);
  _acceleration.resize(Eigen::NoChange, newSize);
  _mass.resize(newSize, 0.0f);
}
