#include "sphere.h"

#include <Eigen/Dense>

#include "cloth.h"
#include "configs.h"

namespace {
void generateVertices(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
  // See http://www.songho.ca/opengl/gl_sphere.html#sphere if you don't know how to create a sphere.
  vertices.reserve(8 * (sphereStack + 1) * (sphereSlice + 1));
  indices.reserve(6 * sphereStack * sphereSlice);

  float x, y, z, xy;  //  position

  float sectorStep = static_cast<float>(EIGEN_PI * 2 / sphereSlice);
  float stackStep = static_cast<float>(EIGEN_PI / sphereStack);
  float sectorAngle, stackAngle;

  for (int i = 0; i <= sphereStack; ++i) {
    stackAngle = static_cast<float>(EIGEN_PI / 2 - i * stackStep);  // [pi/2, -pi/2]
    xy = cosf(stackAngle);                                          // r * cos(u)
    z = sinf(stackAngle);                                           // r * sin(u)

    for (int j = 0; j <= sphereSlice; ++j) {
      sectorAngle = j * sectorStep;  // [0, 2pi]

      x = xy * cosf(sectorAngle);  // r * cos(u) * cos(v)
      y = xy * sinf(sectorAngle);  // r * cos(u) * sin(v)
      vertices.insert(vertices.end(), {x, y, z, x, y, z});
    }
  }

  unsigned int k1, k2;  // EBO index
  for (int i = 0; i < sphereStack; ++i) {
    k1 = i * (sphereSlice + 1);  // beginning of current sphereStack
    k2 = k1 + sphereSlice + 1;   // beginning of next sphereStack
    for (int j = 0; j < sphereSlice; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices.insert(indices.end(), {k1, k2, k1 + 1});
      }
      // k1+1 => k2 => k2+1
      if (i != (sphereStack - 1)) {
        indices.insert(indices.end(), {k1 + 1, k2, k2 + 1});
      }
    }
  }
}
}  // namespace

Spheres& Spheres::initSpheres() {
  static Spheres spheres;
  return spheres;
}

void Spheres::addSphere(const Eigen::Ref<const Eigen::Vector4f>& position, float size) {
  if (sphereCount == _particles.getCapacity()) {
    _particles.resize(sphereCount * 2);
    _radius.resize(sphereCount * 2);
    offsets.allocate(8 * sphereCount * sizeof(float));
    sizes.allocate(2 * sphereCount * sizeof(float));
  }
  _radius[sphereCount] = size;
  _particles.position(sphereCount) = position;
  _particles.velocity(sphereCount).setZero();
  _particles.acceleration(sphereCount).setZero();
  _particles.mass(sphereCount) = sphereDensity * size * size * size;

  sizes.load(0, _radius.size() * sizeof(float), _radius.data());
  ++sphereCount;
}

Spheres::Spheres() : Shape(1, 1), sphereCount(0), _radius(1, 0.0f) {
  offsets.allocate(4 * sizeof(float));
  sizes.allocate(sizeof(float));

  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
  generateVertices(vertices, indices);

  vbo.allocate_load(vertices.size() * sizeof(GLfloat), vertices.data());
  ebo.allocate_load(indices.size() * sizeof(GLuint), indices.data());

  vao.bind();
  vbo.bind();
  ebo.bind();

  vao.enable(0);
  vao.setAttributePointer(0, 3, 6, 0);
  glVertexAttribDivisor(0, 0);
  vao.enable(1);
  vao.setAttributePointer(1, 3, 6, 3);
  glVertexAttribDivisor(1, 0);
  offsets.bind();
  vao.enable(2);
  vao.setAttributePointer(2, 3, 4, 0);
  glVertexAttribDivisor(2, 1);
  sizes.bind();
  vao.enable(3);
  vao.setAttributePointer(3, 1, 1, 0);
  glVertexAttribDivisor(3, 1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Spheres::draw() const {
  vao.bind();
  offsets.load(0, 4 * sphereCount * sizeof(GLfloat), _particles.getPositionData());
  GLsizei indexCount = static_cast<GLsizei>(ebo.getSize() / sizeof(GLuint));
  glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, sphereCount);
  glBindVertexArray(0);
}

void Spheres::collide(Shape* shape) { shape->collide(this); }
void Spheres::collide(Cloth* cloth) {
  constexpr float coefRestitution = 0.0f;

  for (int i = 0; i < sphereCount; ++i) {
    float inverseSphereMass = _particles.inverseMass(i);
    for (int j = 0; j < particlesPerEdge * particlesPerEdge; ++j) {
      float inverseClothMass = cloth->particles().inverseMass(j);
      Eigen::Vector4f normal = _particles.position(i) - cloth->particles().position(j);
      Eigen::Vector4f relativeVelocity = _particles.velocity(i) - cloth->particles().velocity(j);
      float distance = normal.norm();
      float penetration = _radius[i] - distance;
      if (penetration < 0) continue;
      normal.normalize();
      Eigen::Vector4f normalVelocity = normal * relativeVelocity.dot(normal);

      if (relativeVelocity.dot(normal) < 0) {
        Eigen::Vector4f impulse = -(1 + coefRestitution) * normalVelocity / (inverseClothMass + inverseSphereMass);
        _particles.velocity(i) += impulse * inverseSphereMass;
        cloth->particles().velocity(j) -= impulse * inverseClothMass;
      }
      Eigen::Vector4f correction = penetration * normal * 0.15f;
      _particles.position(i) += correction;
      cloth->particles().position(j) -= correction;
    }
  }
}

void Spheres::collide() {
  constexpr float coefRestitution = 0.8f;

  for (int i = 0; i < sphereCount; ++i) {
    float inverseSphere1Mass = _particles.inverseMass(i);
    for (int j = i + 1; j < sphereCount; ++j) {
      float inverseSphere2Mass = _particles.inverseMass(j);

      Eigen::Vector4f normal = _particles.position(i) - _particles.position(j);
      Eigen::Vector4f relativeVelocity = _particles.velocity(i) - _particles.velocity(j);
      float distance = normal.norm();
      float penetration = (_radius[i] + _radius[j]) - distance;
      if (penetration < 0) continue;
      normal.normalize();
      Eigen::Vector4f normalVelocity = normal * relativeVelocity.dot(normal);

      if (relativeVelocity.dot(normal) < 0) {
        Eigen::Vector4f impulse = -(1 + coefRestitution) * normalVelocity / (inverseSphere1Mass + inverseSphere2Mass);
        _particles.velocity(i) += impulse * inverseSphere1Mass;
        _particles.velocity(j) -= impulse * inverseSphere2Mass;
      }
      Eigen::Vector4f correction = penetration * normal * 0.15f;
      _particles.position(i) += correction;
      _particles.position(j) -= correction;
    }
  }
}
