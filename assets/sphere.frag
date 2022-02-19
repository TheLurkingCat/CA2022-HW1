#version 330 core
layout(location = 0) out vec4 FragColor;

in VS_OUT {
  vec3 position;
  vec3 normal;
  flat vec3 viewPosition;
} fs_in;


void main() {
  FragColor = vec4(0.3, 0.3, 0.0, 1.0);
}
