#include "configs.h"

float mouseMoveSpeed = 0.001f;
float keyboardMoveSpeed = 0.1f;

int windowWidth = 0;
int windowHeight = 0;

float deltaTime = 1e-5f;
int simulationPerFrame = static_cast<int>(baseSpeed / deltaTime);

float springCoef = 25000.0f;
float damperCoef = 250.0f;
