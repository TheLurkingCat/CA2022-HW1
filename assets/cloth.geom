#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (std140) uniform model {
  mat4 modelMatrix;
  mat4 normalMatrix;
};

in VS_OUT {
  vec4 position;
} gs_in[3];

out GS_OUT {
  vec4 position;
  vec4 normal;
} gs_out[3];

void main() {
  vec3 a = (gs_in[1].position - gs_in[0].position).xyz;
  vec3 b = (gs_in[2].position - gs_in[0].position).xyz;
  vec3 n = cross(a, b);
  vec4 normal = vec4(normalize(mat3(normalMatrix) * n), 0);

  for (int i = 0; i < 3; ++i){
    gs_out[i].position = modelMatrix * gs_in[i].position;
    gs_out[i].normal = normal;
    gl_Position = gl_in[i].gl_Position;
    EmitVertex();
  }
  EndPrimitive();
}
