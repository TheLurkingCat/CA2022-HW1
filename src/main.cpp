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

#include "glcontext.h"
// Trace some global state
int windowWidth = 0;
int windowHeight = 0;

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
  // Minimize event guard
  if (width == 0 && height == 0) return;
  windowWidth = width;
  windowHeight = height;
  glViewport(0, 0, width, height);
}

int main() {
  // Initialize OpenGL context, details are wrapped in class.
  OpenGLContext& context = OpenGLContext::getContext();
  // context.createWindow();
  GLFWwindow* window = context.createWindow("HW1", 1280, 720, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
  glContext.printSystemInfo();
  glContext.enableDebugCallback();
#endif

  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
  glViewport(0, 0, windowWidth, windowHeight);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0, 0, 0, 1);

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}
