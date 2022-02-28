#include "camera.h"

#include "configs.h"
#include "utils.h"

using Eigen::Matrix4f;
using Eigen::Vector3f;
using Eigen::Vector4f;

namespace {}  // namespace

Camera::Camera(const Eigen::Ref<const Eigen::Vector4f>& _position) :
    position(_position),
    up(Vector4f::UnitY()),
    front(Vector4f::UnitZ()),
    right(Vector4f::UnitX()),
    projectionMatrix(Matrix4f::Identity()),
    viewMatrix(Matrix4f::Identity()),
    rotation(Eigen::Quaternionf::Identity()) {
  updateView();
  updateProjection();
}

bool Camera::move(GLFWwindow* window) {
  bool ismoved = false;
  // Mouse part
  static double lastx = 0, lasty = 0;
  if (lastx == 0 && lasty == 0) {
    glfwGetCursorPos(window, &lastx, &lasty);
  } else {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    float dx = mouseMoveSpeed * static_cast<float>(xpos - lastx);
    float dy = mouseMoveSpeed * static_cast<float>(ypos - lasty);
    lastx = xpos;
    lasty = ypos;
    if (dx != 0 || dy != 0) {
      ismoved = true;
      auto rx = Eigen::AngleAxisf(dx, -Vector3f::UnitY());
      auto ry = Eigen::AngleAxisf(dy, Vector3f::UnitX());
      rotation = rx * rotation * ry;
      rotation.normalize();
    }
  }
  // Keyboard part
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    position += front * keyboardMoveSpeed;
    ismoved = true;
  } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    position -= front * keyboardMoveSpeed;
    ismoved = true;
  } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    position -= right * keyboardMoveSpeed;
    ismoved = true;
  } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    position += right * keyboardMoveSpeed;
    ismoved = true;
  }
  // Update view matrix if moved
  if (ismoved) {
    updateView();
  }
  return ismoved;
}

void Camera::updateView() {
  front.head<3>() = rotation * Vector3f::UnitZ();
  up.head<3>() = rotation * Vector3f::UnitY();
  right = front.cross3(up);
  viewMatrix = lookAt(position, front, up);
}

void Camera::updateProjection() {
  constexpr float FOV = toRadians(45.0);
  constexpr float zNear = 0.1f;
  constexpr float zFar = 100.0f;
  float aspectRatio = static_cast<float>(windowWidth) / windowHeight;
  projectionMatrix = perspective(FOV, aspectRatio, zNear, zFar);
}
