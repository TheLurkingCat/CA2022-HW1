#pragma once
// constants
constexpr int particlesPerEdge = 20;
constexpr int clothWidth = 4;
constexpr int clothHeight = 4;

constexpr int sphereSlice = 360;
constexpr int sphereStack = 180;

// variables

extern int windowWidth;
extern int windowHeight;

extern float mouseMoveSpeed;
extern float keyboardMoveSpeed;

extern float deltaTime;

extern float springCoef;
extern float damperCoef;
