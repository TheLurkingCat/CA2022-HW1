#include "sphere.h"

#include "configs.h"
#include "integrator.h"

namespace {
void generateVertices(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
  // See http://www.songho.ca/opengl/gl_sphere.html#sphere if you don't know how to create a sphere.
  vertices.reserve(8 * (sphereStack + 1) * (sphereSlice + 1));
  indices.reserve(6 * sphereStack * sphereSlice);

  float x, y, z, xy;  //  position
  float s, t;         //  texCoord

  float sectorStep = EIGEN_PI * 2 / sphereSlice;
  float stackStep = EIGEN_PI / sphereStack;
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

void Spheres::addSphere(const Eigen::Vector4f& position) {
  if (sphereCount == particles.getCapacity()) {
    particles.resize(sphereCount * 2);
    offsets.allocate(8 * sphereCount * sizeof(float));
  }
  particles.position(sphereCount) = position;
  particles.velocity(sphereCount).setZero();
  particles.acceleration(sphereCount).setZero();
  ++sphereCount;
}

Spheres::Spheres() : Shape(128), sphereCount(0) {
  offsets.allocate(4 * 128 * sizeof(float));

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
  vao.setAttributePointer(3, 4, 4, 0);
  glVertexAttribDivisor(3, 1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Spheres::draw() const {
  vao.bind();
  GLsizei indexCount = static_cast<GLsizei>(ebo.getSize() / sizeof(GLuint));
  glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, sphereCount);
  glBindVertexArray(0);
}

void Spheres::update(const Integrator& integrator) {
  particles.acceleration().colwise() = Eigen::Vector4f(0, -9.8, 0, 0);
  integrator.integrate(particles);
  offsets.load(0, 4 * sphereCount * sizeof(GLfloat), particles.getPositionData());
}
