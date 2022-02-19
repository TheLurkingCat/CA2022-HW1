#include "integrator.h"

#include "configs.h"

void ExplicitEuler::integrate(Particles &particles) const {
  particles.position() += particles.velocity() * deltaTime;
  particles.velocity() += particles.acceleration() * deltaTime;
}
void ImplicitEuler::integrate(Particles &particles) const {}
void MidpointEuler::integrate(Particles &particles) const {}
void RungeKuttaFourth::integrate(Particles &particles) const {}
