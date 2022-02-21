#include "sphere.h"

#include <Eigen/Dense>

#include "cloth.h"
#include "configs.h"
#include "integrator.h"

namespace {
void generateVertices(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
  // See http://www.songho.ca/opengl/gl_sphere.html#sphere if you don't know how to create a sphere.
  vertices.reserve(8 * (sphereStack + 1) * (sphereSlice + 1));
  indices.reserve(6 * sphereStack * sphereSlice);

  float x, y, z, xy;  //  position
  float s, t;         //  texCoord

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

      // vertex tex coord (s, t) range between [0, 1]
      s = static_cast<float>(j) / sphereSlice;
      t = static_cast<float>(i) / sphereStack;

      vertices.insert(vertices.end(), {x, y, z, x, y, z, s, t});
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

void Spheres::addSphere(const Eigen::Vector4f& position, float size) {
  bool needResize = sphereCount == _particles.getCapacity();
  if (needResize) {
    _particles.resize(sphereCount * 2);
    _radius.resize(sphereCount * 2);
  }
  _radius[sphereCount] = size;
  _particles.position(sphereCount) = position;
  _particles.velocity(sphereCount).setZero();
  _particles.acceleration(sphereCount).setZero();
  _particles.mass(sphereCount) = 1000 * size * size * size;

  if (needResize) {
    offsets.allocate(8 * sphereCount * sizeof(float));
    sizes.allocate_load(2 * sphereCount * sizeof(float), _radius.data());
  }
  ++sphereCount;
}

Spheres::Spheres() : Shape(1, 1), sphereCount(0), _radius(1, 0.0f) {
  offsets.allocate(4 * sizeof(float));
  sizes.allocate(sizeof(float), GL_STATIC_DRAW);

  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
  generateVertices(vertices, indices);

  vbo.allocate_load(vertices.size() * sizeof(GLfloat), vertices.data());
  ebo.allocate_load(indices.size() * sizeof(GLuint), indices.data());

  vao.bind();
  vbo.bind();
  ebo.bind();

  vao.enable(0);
  vao.setAttributePointer(0, 3, 8, 0);
  glVertexAttribDivisor(0, 0);
  vao.enable(1);
  vao.setAttributePointer(1, 3, 8, 3);
  glVertexAttribDivisor(1, 0);
  vao.enable(2);
  vao.setAttributePointer(2, 2, 8, 6);
  glVertexAttribDivisor(2, 0);
  offsets.bind();
  vao.enable(3);
  vao.setAttributePointer(3, 3, 4, 0);
  glVertexAttribDivisor(3, 1);
  sizes.bind();
  vao.enable(4);
  vao.setAttributePointer(4, 1, 1, 0);
  glVertexAttribDivisor(4, 1);

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

void Spheres::update(const Integrator& integrator) {
  _particles.acceleration().colwise() = Eigen::Vector4f(0, -9.8f, 0, 0);
  integrator.integrate(_particles);
}

void Spheres::collide(Shape* shape) { shape->collide(this); }
void Spheres::collide(Cloth* cloth) {
  constexpr float coefRestitution = 0.0f;

  for (int i = 0; i < sphereCount; ++i) {
    float sphereMass = _particles.mass(i);
    float inverseSphereMass = sphereMass == 0.0f ? 0.0f : 1.0f / sphereMass;
    for (int j = 0; j < particlesPerEdge * particlesPerEdge; ++j) {
      float clothMass = cloth->particles().mass(j);
      float inverseClothMass = clothMass == 0.0f ? 0.0f : 1.0f / clothMass;

      Eigen::Vector4f normal = _particles.position(i) - cloth->particles().position(j);
      Eigen::Vector4f relativeVelocity = _particles.velocity(i) - cloth->particles().velocity(j);
      float distance = normal.norm();
      float penetration = _radius[i] - distance;
      if (penetration < 0) continue;
      normal.normalize();
      auto normalVelocity = normal * relativeVelocity.dot(normal);

      if (relativeVelocity.dot(normal) < 0) {
        auto impulse = -(1 + coefRestitution) * normalVelocity / (inverseClothMass + inverseSphereMass);
        _particles.velocity(i) += impulse * inverseSphereMass;
        cloth->particles().velocity(j) -= impulse * inverseClothMass;
      }
      auto correction = penetration * normal * 0.6f / (inverseClothMass + inverseSphereMass);
      _particles.position(i) += correction * inverseSphereMass;
      cloth->particles().position(j) -= correction * inverseClothMass;
    }
  }
}
void Spheres::collide() {
  constexpr float coefRestitution = 0.8f;

  for (int i = 0; i < sphereCount; ++i) {
    float sphere1Mass = _particles.mass(i);
    float inverseSphere1Mass = sphere1Mass == 0.0f ? 0.0f : 1.0f / sphere1Mass;
    for (int j = i + 1; j < sphereCount; ++j) {
      float sphere2Mass = _particles.mass(j);
      float inverseSphere2Mass = sphere2Mass == 0.0f ? 0.0f : 1.0f / sphere2Mass;

      Eigen::Vector4f normal = _particles.position(i) - _particles.position(j);
      Eigen::Vector4f relativeVelocity = _particles.velocity(i) - _particles.velocity(j);
      float distance = normal.norm();
      float penetration = (_radius[i] + _radius[j]) - distance;
      if (penetration < 0) continue;
      normal.normalize();
      auto normalVelocity = normal * relativeVelocity.dot(normal);

      if (relativeVelocity.dot(normal) < 0) {
        auto impulse = -(1 + coefRestitution) * normalVelocity / (inverseSphere1Mass + inverseSphere2Mass);
        _particles.velocity(i) += impulse * inverseSphere1Mass;
        _particles.velocity(j) -= impulse * inverseSphere2Mass;
      }
      auto correction = penetration * normal * 0.6f / (inverseSphere1Mass + inverseSphere2Mass);
      _particles.position(i) += correction * inverseSphere1Mass;
      _particles.position(j) -= correction * inverseSphere2Mass;
    }
  }
}
