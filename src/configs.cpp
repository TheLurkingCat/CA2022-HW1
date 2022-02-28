#include "configs.h"
float mouseMoveSpeed = 0.001f;
float keyboardMoveSpeed = 0.1f;

int windowWidth = 0;
int windowHeight = 0;

float deltaTime = 1e-5f;
int simulationPerFrame = static_cast<int>(baseSpeed / deltaTime);

float springCoef = 25000.0f;
float damperCoef = 250.0f;

Eigen::Vector4f sphereColor = Eigen::Vector4f(0.0f, 0.5f, 0.5f, 1.0f);
Eigen::Vector4f clothColor = Eigen::Vector4f(0.5f, 0.0f, 0.0f, 1.0f);

bool isSphereColorChange = false;
bool isClothColorChange = false;
bool isDrawingParticles = false;
bool isDrawingStructuralSprings = true;
bool isDrawingShearSprings = true;
bool isDrawingBendSprings = false;
bool isDrawingCloth = false;
bool isPaused = true;

int currentIntegrator = 0;
