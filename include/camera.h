#pragma once
#include <memory>

#include <GLFW/glfw3.h>
#include <Eigen/Core>
#include <Eigen/Geometry>

class Camera {
 public:
  // The w value of position should be 1
  explicit Camera(const Eigen::Vector4f& position);

  Eigen::Vector4f getFront() const { return front; }
  Eigen::Vector4f getPosition() const { return position; }
  Eigen::Matrix4f getProjectionMatrix() { return projectionMatrix; }
  Eigen::Matrix4f getViewMatrix() { return viewMatrix; }
  Eigen::Matrix4f getViewProjectionMatrix() { return projectionMatrix * viewMatrix; }

  bool move(GLFWwindow* window);
  void updateProjection();
  void updateView();

 private:
  Eigen::Vector4f position;
  Eigen::Vector4f up;
  Eigen::Vector4f front;
  Eigen::Vector4f right;
  Eigen::Matrix4f projectionMatrix;
  Eigen::Matrix4f viewMatrix;
  Eigen::Quaternionf rotation;
};
