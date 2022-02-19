#include "camera.h"

#include "configs.h"

using Eigen::Matrix4f;
using Eigen::Vector3f;
using Eigen::Vector4f;

namespace {
constexpr float toRadians(double x) { return x * EIGEN_PI / 180.0; }

Matrix4f lookAt(const Vector4f& position, const Vector4f& front, const Vector4f& up) {
  Vector4f f = front.normalized();
  Vector4f u = up.normalized();
  Vector4f s = f.cross3(u).normalized();
  u = s.cross3(f);

  Matrix4f mat(Matrix4f::Zero());
  mat.row(0) = s;
  mat.row(1) = u;
  mat.row(2) = -f;
  mat.row(3) << 0, 0, 0, 1;
  mat(0, 3) = -s.dot(position);
  mat(1, 3) = -u.dot(position);
  mat(2, 3) = f.dot(position);
  return mat;
}

Matrix4f perspective(float fov, float aspect, float zNear, float zFar) {
  Eigen::Transform<float, 3, Eigen::Projective> tr;
  tr.matrix().setZero();

  float tanFOV2 = std::tan(fov / 2.0);
  tr(0, 0) = 1.0 / (aspect * tanFOV2);
  tr(1, 1) = 1.0 / (tanFOV2);
  tr(2, 2) = -(zFar + zNear) / (zFar - zNear);
  tr(3, 2) = -1.0;
  tr(2, 3) = -(2.0 * zFar * zNear) / (zFar - zNear);
  return tr.matrix();
}
}  // namespace

Camera::Camera(const Vector4f& _position) :
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
