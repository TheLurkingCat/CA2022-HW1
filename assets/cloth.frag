#version 330 core
layout(location = 0) out vec4 FragColor;

in GS_OUT {
  vec4 position;
  vec4 normal;
} fs_in;

void main() {
  FragColor = vec4(0.3, 0.0, 0.0, 1.0);
}
