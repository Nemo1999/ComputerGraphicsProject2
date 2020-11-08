#version 410
layout(location = 0) in  vec3 box_vert;
layout(location = 1) in  vec3 box_normal;
layout(location = 2) in  vec2 box_text;

out vec3 box_pos_eye;
out vec3 box_normal_eye;
out vec2 box_text_eye;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 project_mat;

void main () {
  box_pos_eye = vec3(view_mat * model_mat * vec4(box_vert, 1.0)) ;
  // note that if we do any scaling in model_mate,
  //then the eye_normal shoud be obtained by transpose(inverse(view_mat * model_mat)) 
  box_normal_eye = normalize(vec3(view_mat * model_mat * vec4(box_normal, 0.0))) ; 
  box_text_eye = box_text;
  
  gl_Position = project_mat * vec4(box_pos_eye,1.0);
}
