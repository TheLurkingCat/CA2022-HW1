#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class GUI {
 public:
  explicit GUI(GLFWwindow* window, int version = 0);
  ~GUI();
  void render();

 private:
  void renderMainPanel();
};
