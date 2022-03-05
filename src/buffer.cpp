#include "buffer.h"

Buffer::Buffer() noexcept : handle(0), size(0) { glGenBuffers(1, &handle); }

Buffer::~Buffer() { glDeleteBuffers(1, &handle); }

void Buffer::bind() const noexcept { glBindBuffer(getType(), handle); }

void Buffer::allocate(GLsizeiptr _size, GLenum usage) const noexcept {
  bind();
  size = _size;
  glBufferData(getType(), size, nullptr, usage);
}

void Buffer::load(GLintptr offset, GLsizeiptr _size, const void* data) const noexcept {
  bind();
  glBufferSubData(getType(), offset, _size, data);
}

void Buffer::allocate_load(GLsizeiptr _size, const void* data, GLenum usage) const noexcept {
  bind();
  size = _size;
  glBufferData(getType(), size, data, usage);
}

void UniformBuffer::bindUniformBlockIndex(GLuint index, GLuint offset, GLuint _size) const noexcept {
  bind();
  glBindBufferRange(GL_UNIFORM_BUFFER, index, handle, offset, _size);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::bindUniformBlockIndex(GLuint index) const noexcept {
  bind();
  glBindBufferBase(GL_UNIFORM_BUFFER, index, handle);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
