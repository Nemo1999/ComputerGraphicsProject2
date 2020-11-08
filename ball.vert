#version 410
layout(location = 0) in  vec3 vertex_position;
layout(location = 1) in  vec2 vertex_text;
out vec3 position_eye;
out vec3 normal_eye;
out vec2 text_eye;
uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 project_mat;
uniform mat4 view_ori_mat;
void main () {
  position_eye = vec3(view_mat * model_mat * vec4(vertex_position,1.0));
  normal_eye = vec3(normalize(view_mat * model_mat * vec4(vertex_position,0.0)));
  text_eye = vertex_text;
  gl_Position =   project_mat *vec4(position_eye,1.0);
  
}
