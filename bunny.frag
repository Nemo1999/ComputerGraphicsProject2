#version 410 
in vec3 box_pos_eye;
in vec3 box_normal_eye;
in vec2 box_text_eye;
uniform mat4 view_mat;
uniform vec3 texture_noise_ramp;
uniform sampler2D box_texture;
uniform sampler2D noise_texture;
uniform sampler2D ramp_texture;
uniform float time;
// fixed point light properties
vec3 light_position_world = vec3 (2.0, 2.0, 4.0);
vec3 Ls = vec3 (0.5, 0.5, 0.5); // white specular colour
vec3 Ld = vec3 (0.8, 0.8, 0.8); // dull white diffuse light colour
vec3 La = vec3 (0.2, 0.2, 0.2); // grey ambient colour
// surface reflectance
vec3 Ks = vec3 (1.0, 1.0,1.0); // fully reflect specular light	    
vec3 Kd = vec3 (1.0, 1.0,1.0); // orange diffuse surface reflectance 
vec3 Ka = vec3 (1.0, 1.0,1.0); // fully reflect ambient light	    
float specular_exponent =100.0; // specular 'power'		    

out vec4 fragment_colour; // final colour of surface

void main () {
  if(texture_noise_ramp.y> 0.5){
    float thresh = min(max(1.5*sin(0.7 * time - 0.5),0),1);
    float mask_pixel = texture(noise_texture, box_text_eye).x;
    if(thresh < mask_pixel){
      discard;
    }
    if(abs(thresh - mask_pixel) < 0.05){
      fragment_colour = vec4(1.0, 0.0, 1.0, 1.0);
      return ;
    }
  }
  vec4 texel;
  if(texture_noise_ramp.x > 0.5)
    texel = texture (box_texture, box_text_eye);
  else 
    texel = vec4(1.0,1.0,1.0,0.0);
  // ambient intensity
  vec3 Ia = La * vec3(texel);
  // diffuse intensity
  
  // raise light position to eye space
  vec3 light_position_eye = vec3 (view_mat * vec4 (light_position_world, 1.0));
  vec3 distance_to_light_eye = light_position_eye - box_pos_eye;
  vec3 direction_to_light_eye = normalize (distance_to_light_eye);
  float dot_prod = dot (direction_to_light_eye, box_normal_eye);
  dot_prod = max (dot_prod, 0.0);
  vec3 Id = Ld * vec3(texel)* dot_prod; // final diffuse intensity
  // specular intensity
 
  vec3 reflection_eye = reflect (-direction_to_light_eye, box_normal_eye);
  vec3 surface_to_viewer_eye = normalize (-box_pos_eye);
  float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
  dot_prod_specular = max (dot_prod_specular, 0.0);
  float specular_factor = pow (dot_prod_specular, specular_exponent);
  vec3 Is = Ls * vec3(texel) * specular_factor; // final specular intensity

  // final colour
  if(texture_noise_ramp[2]> 0.5){
      vec3 ans = Ia+Id+Is;
      ans.x = texture(ramp_texture, vec2(ans.x,ans.x)).x;
      ans.y = texture(ramp_texture, vec2(ans.y,ans.y)).y;
      ans.z = texture(ramp_texture, vec2(ans.z,ans.z)).z;
      fragment_colour = vec4(ans, 0);
  }
  
  else  
  fragment_colour = vec4 (Ia+Id+Is, 1.0);
  
}
