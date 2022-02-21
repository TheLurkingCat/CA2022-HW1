#include "gui.h"

#include "configs.h"

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
  ImGui::SetNextWindowSize(ImVec2(400.0f, 275.0f), ImGuiCond_Once);
  ImGui::SetNextWindowCollapsed(0, ImGuiCond_Once);
  ImGui::SetNextWindowPos(ImVec2(50.0f, 50.0f), ImGuiCond_Once);
  ImGui::SetNextWindowBgAlpha(0.2f);
  if (ImGui::Begin("Configs")) {
    ImGui::Text("----------------------- Part1 -----------------------");
    ImGui::Text("----------------------- Part2 -----------------------");
    ImGui::Text("----------------------- Bonus -----------------------");
    ImGui::Text("----------------------- Other -----------------------");
    ImGui::Text("Current framerate: %.0f", ImGui::GetIO().Framerate);
  }
  ImGui::End();
}
