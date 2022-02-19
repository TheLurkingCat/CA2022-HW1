#version 330 core

layout(location = 0) in vec4 position_in;
layout(location = 1) in vec2 textureCoordinate_in;

out VS_OUT {
  vec4 position;
} vs_out;

layout (std140) uniform model {
  mat4 modelMatrix;
  mat4 normalMatrix;
};

layout (std140) uniform camera {
  mat4 viewProjectionMatrix;
  vec4 viewPosition;
};

void main() {
  vs_out.position = position_in;
  gl_Position = viewProjectionMatrix * modelMatrix * position_in;
}
