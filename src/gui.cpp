#include "gui.h"
#include <cmath>

#include "configs.h"

namespace {
void renderColorPanel() {
  static bool isUsingSphereColor = false;
  static bool isUsingClothColor = false;

  if (ImGui::Begin("Color config")) {
    if (ImGui::Button("Change sphere color")) isUsingSphereColor = !isUsingSphereColor;
    if (ImGui::Button("Change cloth color")) isUsingClothColor = !isUsingClothColor;

    if (isUsingSphereColor) {
      ImGui::SetNextWindowSize(ImVec2(300.0f, 300.0f), ImGuiCond_Once);
      ImGui::SetNextWindowCollapsed(0, ImGuiCond_Once);
      ImGui::SetNextWindowPos(ImVec2(50.0f, 50.0f), ImGuiCond_Once);
      ImGui::SetNextWindowBgAlpha(1.0f);
      if (ImGui::Begin("Sphere color")) {
        isSphereColorChange = ImGui::ColorPicker4("Sphere Color", sphereColor.data());
        ImGui::End();
      }
    }

    if (isUsingClothColor) {
      ImGui::SetNextWindowSize(ImVec2(300.0f, 300.0f), ImGuiCond_Once);
      ImGui::SetNextWindowCollapsed(0, ImGuiCond_Once);
      ImGui::SetNextWindowPos(ImVec2(50.0f, 50.0f), ImGuiCond_Once);
      ImGui::SetNextWindowBgAlpha(1.0f);
      if (ImGui::Begin("Cloth color")) {
        isClothColorChange = ImGui::ColorPicker4("Cloth Color", clothColor.data());
        ImGui::End();
      }
    }
    ImGui::End();
  }
}
}  // namespace

GUI::GUI(GLFWwindow* window, int version) {
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  if (version >= 41) {
    ImGui_ImplOpenGL3_Init("#version 410 core");
  } else {
    ImGui_ImplOpenGL3_Init(nullptr);
  }
}

GUI::~GUI() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void GUI::render() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  renderMainPanel();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::renderMainPanel() {
  static bool isUsingColorPanel = false;
  ImGui::SetNextWindowSize(ImVec2(400.0f, 275.0f), ImGuiCond_Once);
  ImGui::SetNextWindowCollapsed(0, ImGuiCond_Once);
  ImGui::SetNextWindowPos(ImVec2(50.0f, 50.0f), ImGuiCond_Once);
  ImGui::SetNextWindowBgAlpha(0.2f);
  if (ImGui::Begin("Configs")) {
    if (ImGui::InputFloat("mouseMoveSpeed", &mouseMoveSpeed, 1e-4f, 1e-3f, "%.4f")) {
      mouseMoveSpeed = std::max(0.0f, mouseMoveSpeed);
    }
    if (ImGui::InputFloat("keyboardMoveSpeed", &keyboardMoveSpeed, 1e-2f, 1e-1f, "%.2f")) {
      keyboardMoveSpeed = std::max(0.0f, keyboardMoveSpeed);
    }
    if (ImGui::InputFloat("deltaTime", &deltaTime, 1e-6f, 1e-5f, "%.6f")) {
      deltaTime = std::max(0.0f, deltaTime);
      simulationPerFrame = static_cast<int>(baseSpeed / deltaTime);
    }
    if (ImGui::InputFloat("springCoef", &springCoef, 1e2f, 1e3f, "%.0f")) {
      springCoef = std::max(0.0f, springCoef);
    }
    if (ImGui::InputFloat("damperCoef", &damperCoef, 1.0f, 1e2f, "%.0f")) {
      damperCoef = std::max(0.0f, damperCoef);
    }
    if (ImGui::Button("Color Configs")) {
      isUsingColorPanel = !isUsingColorPanel;
    }
    if (isUsingColorPanel) renderColorPanel();
    ImGui::Text("Current framerate: %.0f", ImGui::GetIO().Framerate);
    ImGui::End();
  }
}
