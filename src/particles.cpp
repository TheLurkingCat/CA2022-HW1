#include "particles.h"

Particles::Particles(int size, float mass_) noexcept :
    _position(4, size), _velocity(4, size), _acceleration(4, size), _mass(size, mass_) {
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
