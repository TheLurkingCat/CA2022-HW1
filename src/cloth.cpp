#include "cloth.h"

#include "configs.h"
#include "sphere.h"

Cloth::Cloth() : Shape(particlesPerEdge * particlesPerEdge, particleMass) {
  initializeVertex();
  initializeSpring();
}

void Cloth::draw(DrawType type) const {
  vao.bind();
  positionBuffer.load(0, 4 * particlesPerEdge * particlesPerEdge * sizeof(GLfloat), _particles.getPositionData());
  const ElementArrayBuffer* currentEBO = nullptr;
  switch (type) {
    case DrawType::PARTICLE: [[fallthrough]];
    case DrawType::FULL: currentEBO = &ebo; break;
    case DrawType::STRUCTURAL: currentEBO = &structuralSpring; break;
    case DrawType::SHEAR: currentEBO = &shearSpring; break;
    case DrawType::BEND: currentEBO = &bendSpring;
  }
  currentEBO->bind();
  GLsizei indexCount = static_cast<GLsizei>(currentEBO->getSize() / sizeof(GLuint));
  if (type == DrawType::FULL)
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
  else if (type == DrawType::PARTICLE)
    glDrawArrays(GL_POINTS, 0, particlesPerEdge * particlesPerEdge);
  else
    glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Cloth::initializeVertex() {
  float wStep = 2.0f * clothWidth / (particlesPerEdge - 1);
  float hStep = 2.0f * clothHeight / (particlesPerEdge - 1);
  float wTexStep = 1.0f / (particlesPerEdge - 1);
  float hTexStep = 1.0f / (particlesPerEdge - 1);

  int current = 0;
  std::vector<GLfloat> texCoord;
  for (int i = 0; i < particlesPerEdge; ++i) {
    for (int j = 0; j < particlesPerEdge; ++j) {
      _particles.mass(current) = 1.0f;
      _particles.position(current++) = Eigen::Vector4f(-clothWidth + j * wStep, 0, -clothHeight + i * hStep, 1);
      texCoord.insert(texCoord.end(), {j * wTexStep, 1 - i * hTexStep});
    }
  }
  // Four corners will not move
  _particles.mass(0) = 0.0f;
  _particles.mass(particlesPerEdge - 1) = 0.0f;
  _particles.mass(particlesPerEdge * (particlesPerEdge - 1)) = 0.0f;
  _particles.mass(particlesPerEdge * particlesPerEdge - 1) = 0.0f;

  std::vector<GLuint> indices;
  indices.reserve((particlesPerEdge - 1) * (2 * particlesPerEdge + 1));
  for (int i = 0; i < particlesPerEdge - 1; ++i) {
    int offset = i * (particlesPerEdge);
    for (int j = 0; j < particlesPerEdge - 1; ++j) {
      indices.emplace_back(offset + j);
      indices.emplace_back(offset + j + particlesPerEdge);
      indices.emplace_back(offset + j + 1);

      indices.emplace_back(offset + j + 1);
      indices.emplace_back(offset + j + particlesPerEdge);
      indices.emplace_back(offset + j + particlesPerEdge + 1);
    }
  }

  int vboSize = particlesPerEdge * particlesPerEdge * sizeof(GLfloat);
  texCoordsBuffer.allocate_load(vboSize * 2, texCoord.data());
  positionBuffer.allocate_load(vboSize * 4, _particles.getPositionData());
  ebo.allocate_load(indices.size() * sizeof(GLuint), indices.data());

  vao.bind();
  positionBuffer.bind();
  vao.enable(0);
  vao.setAttributePointer(0, 4, 4, 0);
  texCoordsBuffer.bind();
  vao.enable(1);
  vao.setAttributePointer(1, 2, 2, 0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Cloth::initializeSpring() {
  // 0 1 2 3 ... particlesPerEdge
  // particlesPerEdge + 1 ....
  // ... ... particlesPerEdge * particlesPerEdge - 1
  // Structrual
  float structrualLength = (_particles.position(0) - _particles.position(1)).norm();
  for (int i = 0; i < particlesPerEdge; ++i) {
    for (int j = 0; j < particlesPerEdge - 1; ++j) {
      int index = i * particlesPerEdge + j;
      springs.emplace_back(index, index + 1, structrualLength, Spring::Type::STRUCTURAL);
    }
  }
  for (int i = 0; i < particlesPerEdge - 1; ++i) {
    for (int j = 0; j < particlesPerEdge; ++j) {
      int index = i * particlesPerEdge + j;
      springs.emplace_back(index, index + particlesPerEdge, structrualLength, Spring::Type::STRUCTURAL);
    }
  }
  // Shear
  float shearLength = (_particles.position(0) - _particles.position(particlesPerEdge + 1)).norm();
  for (int i = 0; i < particlesPerEdge - 1; ++i) {
    for (int j = 0; j < particlesPerEdge - 1; ++j) {
      int index = i * particlesPerEdge + j;
      springs.emplace_back(index, index + particlesPerEdge + 1, shearLength, Spring::Type::SHEAR);
    }
  }
  for (int i = 0; i < particlesPerEdge - 1; ++i) {
    for (int j = 1; j < particlesPerEdge; ++j) {
      int index = i * particlesPerEdge + j;
      springs.emplace_back(index, index + particlesPerEdge - 1, shearLength, Spring::Type::SHEAR);
    }
  }
  // Bend
  float bendLength = (_particles.position(0) - _particles.position(2)).norm();
  for (int i = 0; i < particlesPerEdge; ++i) {
    for (int j = 0; j < particlesPerEdge - 2; ++j) {
      int index = i * particlesPerEdge + j;
      springs.emplace_back(index, index + 2, bendLength, Spring::Type::BEND);
    }
  }
  for (int i = 0; i < particlesPerEdge - 2; ++i) {
    for (int j = 0; j < particlesPerEdge; ++j) {
      int index = i * particlesPerEdge + j;
      springs.emplace_back(index, index + 2 * particlesPerEdge, bendLength, Spring::Type::BEND);
    }
  }

  std::vector<GLuint> structrualIndices, shearIndices, bendIndices;
  for (const auto& spring : springs) {
    switch (spring.type()) {
      case Spring::Type::STRUCTURAL:
        structrualIndices.emplace_back(spring.startParticleIndex());
        structrualIndices.emplace_back(spring.endParticleIndex());
        break;
      case Spring::Type::SHEAR:
        shearIndices.emplace_back(spring.startParticleIndex());
        shearIndices.emplace_back(spring.endParticleIndex());
        break;
      case Spring::Type::BEND:
        bendIndices.emplace_back(spring.startParticleIndex());
        bendIndices.emplace_back(spring.endParticleIndex());
        break;
    }
  }
  structuralSpring.allocate_load(structrualIndices.size() * sizeof(GLuint), structrualIndices.data());
  shearSpring.allocate_load(shearIndices.size() * sizeof(GLuint), shearIndices.data());
  bendSpring.allocate_load(bendIndices.size() * sizeof(GLuint), bendIndices.data());
}
void Cloth::computeSpringForce() {
  for (const auto& spring : springs) {
    int startID = spring.startParticleIndex();
    int endID = spring.endParticleIndex();
    Eigen::Vector4f direction = _particles.position(startID) - _particles.position(endID);
    Eigen::Vector4f relativeVelocity = _particles.velocity(startID) - _particles.velocity(endID);
    // Spring force
    float currentLength = direction.norm();               // |Xa - Xb|
    direction.normalize();                                // Unit L
    float deltaLength = currentLength - spring.length();  // deltaL
    // Damper force
    float deltaVelocity = relativeVelocity.dot(direction);
    Eigen::Vector4f internalForce = direction * (damperCoef * deltaVelocity + springCoef * deltaLength);

    if (_particles.mass(startID) != 0) _particles.acceleration(startID) -= internalForce / _particles.mass(startID);
    if (_particles.mass(endID) != 0) _particles.acceleration(endID) += internalForce / _particles.mass(endID);
  }
  // Four edge will not move.
  _particles.acceleration(0).setZero();
  _particles.acceleration(particlesPerEdge - 1).setZero();
  _particles.acceleration(particlesPerEdge * (particlesPerEdge - 1)).setZero();
  _particles.acceleration(particlesPerEdge * particlesPerEdge - 1).setZero();
}

void Cloth::collide(Shape* shape) { shape->collide(this); }
void Cloth::collide(Spheres* sphere) { sphere->collide(this); }
