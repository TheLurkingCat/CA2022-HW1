#include "integrator.h"

#include "configs.h"

void ExplicitEuler::integrate(Particles &particles, std::function<void(void)>) const {
  particles.position() += particles.velocity() * deltaTime;
  particles.velocity() += particles.acceleration() * deltaTime;
}
void ImplicitEuler::integrate(Particles &particles, std::function<void(void)> simulateOneStep) const {}
void MidpointEuler::integrate(Particles &particles, std::function<void(void)> simulateOneStep) const {}
void RungeKuttaFourth::integrate(Particles &particles, std::function<void(void)> simulateOneStep) const {}
