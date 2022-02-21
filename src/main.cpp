#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION

#include "hw1.h"
int alignSize = 256;
bool isWindowSizeChanged = true;
bool mouseBinded = false;

int uboAlign(int i) { return ((i + 1 * (alignSize - 1)) / alignSize) * alignSize; }

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold
  if (action != GLFW_PRESS) return;
  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  } else if (key == GLFW_KEY_F9) {
    // Disable / enable mouse cursor.
    if (mouseBinded)
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    mouseBinded = !mouseBinded;
  }
}

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
  // Minimize event guard
  if (width == 0 && height == 0) return;
  windowWidth = width;
  windowHeight = height;
  glViewport(0, 0, width, height);
  isWindowSizeChanged = true;
}

int main() {
  // Initialize OpenGL context.
  OpenGLContext& context = OpenGLContext::getContext();
  GLFWwindow* window = context.createWindow("HW1", 1280, 720, GLFW_OPENGL_CORE_PROFILE);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

#ifndef NDEBUG
  context.printSystemInfo();
  context.enableDebugCallback();
#endif
  GUI gui(window, context.getOpenGLVersion());

  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
  glViewport(0, 0, windowWidth, windowHeight);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0, 0, 0, 1);
  glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignSize);

  ShaderProgram clothRenderer, sphereRenderer, particleRenderer;
  {
    VertexShader vs;
    GeometryShader gs;
    FragmentShader fs;
    vs.fromFile("../assets/cloth.vert");
    gs.fromFile("../assets/cloth.geom");
    fs.fromFile("../assets/cloth.frag");
    clothRenderer.attach(&vs, &gs, &fs);
    clothRenderer.link();
    clothRenderer.detach(&vs, &gs, &fs);
    clothRenderer.use();

    clothRenderer.uniformBlockBinding("model", 0);
    clothRenderer.uniformBlockBinding("camera", 1);
  }
  {
    VertexShader vs;
    FragmentShader fs;
    vs.fromFile("../assets/sphere.vert");
    fs.fromFile("../assets/sphere.frag");
    sphereRenderer.attach(&vs, &fs);
    sphereRenderer.link();
    sphereRenderer.detach(&vs, &fs);
    sphereRenderer.use();

    sphereRenderer.uniformBlockBinding("model", 0);
    sphereRenderer.uniformBlockBinding("camera", 1);
  }
  {
    VertexShader vs;
    FragmentShader fs;
    vs.fromFile("../assets/particle.vert");
    fs.fromFile("../assets/particle.frag");
    particleRenderer.attach(&vs, &fs);
    particleRenderer.link();
    particleRenderer.detach(&vs, &fs);
    particleRenderer.use();

    particleRenderer.uniformBlockBinding("model", 0);
    particleRenderer.uniformBlockBinding("camera", 1);
  }

  Cloth cloth;
  UniformBuffer meshUBO;
  int meshOffset = uboAlign(32 * sizeof(GLfloat));
  meshUBO.allocate(2 * meshOffset);
  meshUBO.load(0, 16 * sizeof(GLfloat), cloth.getModelMatrix().data());
  meshUBO.load(16 * sizeof(GLfloat), 16 * sizeof(GLfloat), cloth.getNormalMatrix().data());

  Spheres& spheres = Spheres::initSpheres();

  spheres.addSphere(Eigen::Vector4f(-0.75, 2, -0.75, 1), 0.5f);
  spheres.addSphere(Eigen::Vector4f(0.75, 2, -0.75, 1), 0.5f);
  spheres.addSphere(Eigen::Vector4f(-0.75, 2, 0.75, 1), 0.5f);
  spheres.addSphere(Eigen::Vector4f(0.75, 2, 0.75, 1), 0.5f);
  spheres.addSphere(Eigen::Vector4f(0, 4, 0, 1), 0.5f);
  meshUBO.load(meshOffset, 16 * sizeof(GLfloat), spheres.getModelMatrix().data());
  meshUBO.load(meshOffset + 16 * sizeof(GLfloat), 16 * sizeof(GLfloat), spheres.getNormalMatrix().data());

  Camera camera(Eigen::Vector4f(0, 2, -10, 1));
  UniformBuffer cameraUBO;
  cameraUBO.allocate(uboAlign(20 * sizeof(GLfloat)));
  cameraUBO.load(0, 16 * sizeof(GLfloat), camera.getViewProjectionMatrix().data());
  cameraUBO.load(16 * sizeof(GLfloat), 4 * sizeof(GLfloat), camera.getPosition().data());
  cameraUBO.bindUniformBlockIndex(1, 0, uboAlign(20 * sizeof(GLfloat)));

  ExplicitEuler explicitEuler;
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bool cameraChanged = mouseBinded ? camera.move(window) : false;
    if (isWindowSizeChanged) {
      isWindowSizeChanged = false;
      camera.updateProjection();
      cameraChanged = true;
    }
    if (cameraChanged) {
      cameraUBO.load(0, 16 * sizeof(GLfloat), camera.getViewProjectionMatrix().data());
      cameraUBO.load(16 * sizeof(GLfloat), 4 * sizeof(GLfloat), camera.getPosition().data());
    }
    for (int i = 0; i < simulationPerFrame; i++) {
      cloth.update(explicitEuler);
      spheres.update(explicitEuler);
      spheres.collide(&cloth);
      spheres.collide();
    }

    particleRenderer.use();
    meshUBO.bindUniformBlockIndex(0, 0, meshOffset);
    cloth.draw(Cloth::DrawType::STRUCTURAL);
    cloth.draw(Cloth::DrawType::SHEAR);
    // clothRenderer.use();
    // glDisable(GL_CULL_FACE);
    // cloth.draw(Cloth::DrawType::FULL);
    // glEnable(GL_CULL_FACE);
    sphereRenderer.use();
    meshUBO.bindUniformBlockIndex(0, meshOffset, meshOffset);
    spheres.draw();

    gui.render();
#ifdef __APPLE__
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  glfwDestroyWindow(window);
  return 0;
}
