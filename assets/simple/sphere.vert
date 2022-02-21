#version 330 core

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 textureCoordinate_in;
layout(location = 3) in vec3 offset_in;
layout(location = 4) in float radius_in;

layout (std140) uniform model {
  mat4 modelMatrix;
  mat4 normalMatrix;
};

layout (std140) uniform camera {
  mat4 viewProjectionMatrix;
  vec4 viewPosition;
};

void main() {
  vec4 transformed = (modelMatrix * (vec4(radius_in * position_in, 1.0))) + vec4(offset_in, 0.0);
  gl_Position = viewProjectionMatrix * transformed;
}
