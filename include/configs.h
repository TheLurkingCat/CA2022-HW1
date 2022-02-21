#pragma once
#include <Eigen/Core>
// constants
inline constexpr int particlesPerEdge = 25;
inline constexpr int clothWidth = 2;
inline constexpr int clothHeight = 2;
inline constexpr float particleMass = (clothWidth * clothHeight) * 2.5f / (particlesPerEdge * particlesPerEdge);
inline constexpr float sphereDensity = 1000.0f;
inline constexpr float baseSpeed = 1e-3f;

inline constexpr int sphereSlice = 360;
inline constexpr int sphereStack = 180;

// variables

extern int windowWidth;
extern int windowHeight;

extern float mouseMoveSpeed;
extern float keyboardMoveSpeed;

extern float deltaTime;
extern int simulationPerFrame;

extern float springCoef;
extern float damperCoef;

extern Eigen::Vector4f sphereColor;
extern Eigen::Vector4f clothColor;

extern bool isSphereColorChange;
extern bool isClothColorChange;
