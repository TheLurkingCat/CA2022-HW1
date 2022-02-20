#pragma once
// constants
constexpr int particlesPerEdge = 25;
constexpr int clothWidth = 2;
constexpr int clothHeight = 2;
constexpr float particleMass = (clothWidth * clothHeight) * 2.5f / (particlesPerEdge * particlesPerEdge);
constexpr float baseSpeed = 1e-3f;

constexpr int sphereSlice = 360;
constexpr int sphereStack = 180;

// variables

extern int windowWidth;
extern int windowHeight;

extern float mouseMoveSpeed;
extern float keyboardMoveSpeed;

extern float deltaTime;
extern int simulationPerFrame;

extern float springCoef;
extern float damperCoef;
