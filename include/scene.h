#pragma once
#include <vector>
class Shape;

class Scene {
 public:
  void collide();

 private:
  std::vector<Shape*> shapes;
};
