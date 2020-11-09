#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <vector>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <stdbool.h>
#include "LinAlg.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "loadOBJ.hpp"

#define GL_LOG_FILE "gl.log"
#ifndef PI
#define PI 3.14159265358979
#endif
#define CIRC_RES 30
#define BALL_RES 30
#define CAMERA_ORI 10;
#define OBJECT_ORI 11;

bool gl_log (const char* message, ...);
bool restart_gl_log ();
bool gl_log_err (const char* message, ...);
void glfw_error_callback (int error, const char* description);
void log_gl_params ();
void _prin_shader_info_log (GLuint shader_index);
void _print_programme_info_log (GLuint programme);
void print_all (GLuint programme);
const char* GL_type_to_string (GLenum type);  
bool is_valid (GLuint programme);
GLuint loadShader(char* filename,GLenum shaderType);
GLuint linkShaders(GLuint* shaders , unsigned short num);
void _update_fps_counter (GLFWwindow* window);
void loadtexture(char* filename, unsigned short index);

// reported window size may be good to know for a few things
int g_win_width = 640;
int g_win_height = 480;

// keep track of framebuffer size for things like the viewport and the mouse cursor
int g_fb_width = 640;
int g_fb_height = 480;
float aspect_ratio = 640.0/480.0;

float camera_ori[3] = {0.0f,0.0f,0.0f};  
float prev_camera_ori[3] = {0.0f,0.0f,0.0f};

float moon_ori[3] = {0.0f,0.0f,0.0f};
float earth_ori[3] = {0.0f,0.0f,0.0f};
float box_ori[3] = {0.0f,0.0f,0.0f};
float earth_pos[3] = {0.0f,0.0f,0.0f};
float bunny_ori[3] = {0.0f,0.0f,0.0f};
float* oris[5] = {0,box_ori,earth_ori,moon_ori,bunny_ori};
float prev_obj_ori[3] = {0.0f,0.0f,0.0f};

float texture_noise_ramp_control[3] = {0.0f,0.0f,0.0f};

int ori_control_mode = 1 ;
bool show_box=false;
bool show_earth=false;
bool show_moon=true;
bool show_bunny=true;
int last_shown=4;

double previous_seconds;
int frame_count;
double mouse_x_before_press;
double mouse_y_before_press;
bool mouse_is_down = 0;
/* we will use this function to update the window title with a frame rate */




void glfw_window_size_callback (GLFWwindow* window, int width, int height){
  g_win_width = width;
  g_win_height = height;
  aspect_ratio = (float) g_fb_width / (float)g_fb_height;
  gl_log("window_size changed to %dx%d\n" , width,height);
		       
}
void glfw_framebuffer_resize_callback (GLFWwindow* window, int width, int height) {
  g_fb_width = width;
  g_fb_height = height;
  aspect_ratio = (float) g_fb_width / (float)g_fb_height;
  gl_log("framebuffer_size changed to %dx%d\n", width , height);
  /* later update any perspective matrices used here */
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
    glfwGetCursorPos(window, &mouse_x_before_press, &mouse_y_before_press);
    mouse_is_down = 1;
  }
  else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
    mouse_is_down = 0;
  }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{ 
  if( ori_control_mode){
    if (mouse_is_down)
      {
        camera_ori[1] = prev_camera_ori[1] + PI*(xpos - mouse_x_before_press)/g_fb_width;
        camera_ori[0] = prev_camera_ori[0] + PI*(ypos - mouse_y_before_press)/g_fb_height;
      }
    else{
      prev_camera_ori[1] = camera_ori[1];
      prev_camera_ori[0] = camera_ori[0];
    }
  }
  else{// ori_control_mode  = OBJ_ORI
    if (mouse_is_down)
      {
        oris[last_shown][1] = prev_obj_ori[1] + PI*(xpos - mouse_x_before_press)/g_fb_width;
        oris[last_shown][0] = prev_obj_ori[0] + PI*(ypos - mouse_y_before_press)/g_fb_height;
      }
    else{
      prev_obj_ori[1] = oris[last_shown][1];
      prev_obj_ori[0] = oris[last_shown][0];
    }
  }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_1 && action == GLFW_PRESS){
    show_box = !show_box;
    if(show_box)last_shown = 1;
  }
  if (key == GLFW_KEY_2 && action == GLFW_PRESS){
    show_earth = !show_earth;
    if(show_earth)last_shown = 2;
  }
  if (key == GLFW_KEY_3 && action == GLFW_PRESS){
    show_moon = !show_moon;
    if(show_moon)last_shown = 3;
  }
  if (key == GLFW_KEY_4 && action == GLFW_PRESS){
    show_bunny = !show_bunny;
    if(show_bunny)last_shown = 4;
  }
  if (key == GLFW_KEY_TAB && action == GLFW_PRESS ){
    if( ori_control_mode ){ 
      ori_control_mode = 0 ;
    }
    else{
      ori_control_mode = 1 ;
    }
  }
  if (key == GLFW_KEY_7 && action == GLFW_PRESS){
    texture_noise_ramp_control[0] = 1.0 -texture_noise_ramp_control[0];
  }
  if (key == GLFW_KEY_8 && action == GLFW_PRESS){
    texture_noise_ramp_control[1] = 1.0 -texture_noise_ramp_control[1];
  }
  if (key == GLFW_KEY_9 && action == GLFW_PRESS){
    texture_noise_ramp_control[2] = 1.0 -texture_noise_ramp_control[2];
  }
}

int main () {
  
  //==============================initialize glfw and glew========================
  if (!restart_gl_log ()) { /* quit? */ }
  // start GL context and O/S window using the GLFW helper library
  gl_log ("starting GLFW\n%s\n", glfwGetVersionString ());
  // register the error call-back function that we wrote, above
  glfwSetErrorCallback (glfw_error_callback);
  if (!glfwInit ()) {
    fprintf (stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }
  // uncomment these lines if on Apple OS X
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // gl anti-aliasing filter samples 16 is for screen shot , 4 is normally  good. 
    glfwWindowHint (GLFW_SAMPLES, 8);


  GLFWmonitor* mon = glfwGetPrimaryMonitor ();
  const GLFWvidmode* vmode = glfwGetVideoMode (mon);
  //GLFWwindow* window = glfwCreateWindow (vmode->width, vmode->height, "Extended GLInit", mon, NULL);

  GLFWwindow* window = glfwCreateWindow (640, 480, "Hello Triangle", NULL, NULL);
  if (!window) {
    fprintf (stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwSetWindowSizeCallback (window, glfw_window_size_callback);
  glfwSetFramebufferSizeCallback (window, glfw_framebuffer_resize_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetKeyCallback(window, key_callback);
  
  glfwMakeContextCurrent (window);
  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit ();

  log_gl_params();
  // get version info
  const GLubyte* renderer = glGetString (GL_RENDERER);
  const GLubyte* version = glGetString (GL_VERSION);
  printf ("Renderer: %s\n", renderer);
  printf ("OpenGL version supported %s\n", version);
  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable (GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"

//======================load textures=======================================

loadtexture("Resources/Old/container.jpg",0);
loadtexture("Resources/Old/earth.jpg",1);
loadtexture("Resources/Old/moon.jpg",2);
loadtexture("Resources/Stone.ppm",3);
loadtexture("Resources/noisetexture.ppm.bmp",4);
loadtexture("Resourdes/ramp.ppm",5);
//=====================load vertex, normal, and texture from obj file==============


std::vector < glm::vec3 >  bunny_vertices;
std::vector < glm::vec2 >  bunny_uvs;
std::vector < glm::vec3 >  bunny_normals;
readOBJ("Resources/bunny.obj", bunny_vertices , bunny_uvs, bunny_normals);

  
//===================== generating vertex, normal and texture data======================



  GLfloat ball_vert[(BALL_RES+1)*(BALL_RES+1)*3];
  for(int i=0;i<BALL_RES+1;i++){
    for(int j =0;j<BALL_RES+1;j++){
      float latitude = (PI/(float)BALL_RES)*i - (float)PI/2.0;
      float longtitude = (2*PI/(float)BALL_RES)*j;
      ball_vert[(i*(BALL_RES+1)+j)*3] = cos(latitude)*cos(longtitude);
      ball_vert[(i*(BALL_RES+1)+j)*3+1] = cos(latitude)*sin(longtitude);
      ball_vert[(i*(BALL_RES+1)+j)*3+2] = sin(latitude);
    }
  }
/*
  for(int i=0;i<BALL_RES*BALL_RES;i++)
    printf("%f\t%f\t%f\n",ball_vert[i*3],ball_vert[i*3+1],ball_vert[i*3+2]);
*/  
  GLfloat ball_text[(BALL_RES+1)*(BALL_RES+1)*2];
  for(int i=0;i<BALL_RES+1;i++){
    for(int j =0;j<BALL_RES+1;j++){
      float res = 1.0/(float)BALL_RES;
      ball_text[(i*(BALL_RES+1)+j)*2] = res*j;
      ball_text[(i*(BALL_RES+1)+j)*2+1] = res*i;
      
    }
  }

  GLuint ball_indices[(BALL_RES+1)*(2*BALL_RES)];
    for(int i=0;i<BALL_RES;i++){
      for(int j=0;j<BALL_RES+1;j++){
	      ball_indices[i*(BALL_RES+1)*2+j*2]   =i*(BALL_RES+1) + j;
	      ball_indices[i*(BALL_RES+1)*2+j*2+1] =(i+1)*(BALL_RES+1) + j;
      }
    }

  //box vertex
  GLfloat box_vert[36*3];
  for(int fix_cord=0;fix_cord<3;fix_cord++){ // fix x y or z coord
    for(int fix = 0; fix<2;fix++){// fix cord 1 or -1
      GLfloat* box_square = &(box_vert[fix_cord*12*3+fix*6*3]);
      for(short i=0;i<6;i++){// points in a square
	      short square_2D[6][2]={
			    {1,1},
			    {1,-1},
			    {-1,1},
			    {-1,-1},
			    {-1,1},
			    {1,-1}
	      };
	      short pass_fix = 0;
	      for(short j=0;j<3;j++){//xyz channel
	        if(j==fix_cord){
	          box_square[i*3+fix_cord] =(float)fix?1:-1;
	          pass_fix = 1;
	        }
	        else{
	          box_square[i*3+j] = (float) square_2D[i][j - pass_fix];
	        }
	  
	      }
      }
    }
  } 
  GLfloat box_normal[36*3];
  for(int fix_cord=0;fix_cord<3;fix_cord++){ // fix x y or z coord
    for(int fix = 0; fix<2;fix++){// fix cord 1 or -1
        GLfloat* box_square = &(box_normal[fix_cord*12*3+fix*6*3]);
        for(short i=0;i<6;i++){// points in a square
	        for(short j=0;j<3;j++){
	          box_square[i*3+j] = (j==fix_cord)? (float)(fix?1:-1): (float)0;
	        }
        }
    }  
  }
  GLfloat box_text[36*2];
  for(int fix_cord=0;fix_cord<3;fix_cord++){ // fix x y or z coord
    for(int fix = 0; fix<2;fix++){// fix cord 1 or -1
      GLfloat* box_square = &(box_text[fix_cord*12*2+fix*6*2]);
      for(short i=0;i<6;i++){// points in a square
	      short square_2D[6][2]={
			       {1,1},
			       {1,0},
			       {0,1},
			       {0,0},
			       {0,1},
			       {1,0}
	      };
	    box_square[i*2] = square_2D[i][0];
	    box_square[i*2+1] = square_2D[i][1];
      }
    }
  }

//=========================load data into VBO , VAO ======================================

//--------------bunny
GLuint vbo_bunny_vert = 0;
    glGenBuffers (1,&vbo_bunny_vert);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_bunny_vert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*bunny_vertices.size(),&(bunny_vertices[0]),GL_STATIC_DRAW);

  GLuint vbo_bunny_normal = 0;
    glGenBuffers (1,&vbo_bunny_normal);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_bunny_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*bunny_normals.size(),&(bunny_normals[0]),GL_STATIC_DRAW);

  GLuint vbo_bunny_text = 0;
    glGenBuffers (1,&vbo_bunny_text);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_bunny_text);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*bunny_uvs.size(),&(bunny_uvs[0]),GL_STATIC_DRAW);
  
  GLuint vao_bunny = 0;
    glGenVertexArrays (1, &vao_bunny);
    glBindVertexArray (vao_bunny);

    glEnableVertexAttribArray (0);//set layout (location = 0) in vertex shader 
    glBindBuffer (GL_ARRAY_BUFFER, vbo_bunny_vert);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  
    glEnableVertexAttribArray (1);//set layout (location = 1) in vertex shader 
    glBindBuffer (GL_ARRAY_BUFFER, vbo_bunny_normal);
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray (2);//set layout (location = 2) in vertex shader 
    glBindBuffer (GL_ARRAY_BUFFER, vbo_bunny_text);
    glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  
    //unbind the VAO
    glBindVertexArray(0);
    //unbind buffers to check that VAO work fine
    glBindBuffer(GL_ARRAY_BUFFER,0); 

// ------------------box

  GLuint vbo_box_vert = 0;
    glGenBuffers (1,&vbo_box_vert);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_box_vert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box_vert),box_vert,GL_STATIC_DRAW);

  GLuint vbo_box_normal = 0;
    glGenBuffers (1,&vbo_box_normal);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_box_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box_normal),box_normal,GL_STATIC_DRAW);

  GLuint vbo_box_text = 0;
    glGenBuffers (1,&vbo_box_text);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_box_text);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box_text),box_text,GL_STATIC_DRAW);

  
  
  GLuint vao_box = 0;
    glGenVertexArrays (1, &vao_box);
    glBindVertexArray (vao_box);

    glEnableVertexAttribArray (0);//set layout (location = 0) in vertex shader 
    glBindBuffer (GL_ARRAY_BUFFER, vbo_box_vert);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  
    glEnableVertexAttribArray (1);//set layout (location = 1) in vertex shader 
    glBindBuffer (GL_ARRAY_BUFFER, vbo_box_normal);
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray (2);//set layout (location = 2) in vertex shader 
    glBindBuffer (GL_ARRAY_BUFFER, vbo_box_text);
    glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  
    //unbind the VAO
    glBindVertexArray(0);
    //unbind buffers to check that VAO work fine
    glBindBuffer(GL_ARRAY_BUFFER,0); 
  
//---------------ball
  GLuint vbo_ball_vert = 0;
    glGenBuffers (1,&vbo_ball_vert);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_ball_vert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ball_vert),ball_vert,GL_STATIC_DRAW);
  GLuint vbo_ball_text = 0;
    glGenBuffers (1,&vbo_ball_text);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_ball_text);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ball_text),ball_text,GL_STATIC_DRAW);
  GLuint vbo_ball_ind = 0;
    glGenBuffers (1,&vbo_ball_ind);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo_ball_ind);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ball_indices),ball_indices,GL_STATIC_DRAW);
  
  GLuint vao_ball = 0;
    glGenVertexArrays (1, &vao_ball);
    glBindVertexArray (vao_ball);

    glEnableVertexAttribArray (0);//set layout (location = 0) in vertex shader 
    glBindBuffer (GL_ARRAY_BUFFER, vbo_ball_vert);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  
    glEnableVertexAttribArray (1);//set layout (location = 1) in vertex shader 
    glBindBuffer (GL_ARRAY_BUFFER, vbo_ball_text);
    glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ball_ind);
    //the following line is not needed for indexed buffer
    //glVertexAttribPointer (0, 1, GL_UNSIGNED_SHORT, GL_FALSE, 0, NULL);
    //unbind the VAO
    glBindVertexArray(0);
    //unbind buffers to check that VAO work fine
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);




//================================load shaders , get uniform locations ========================================

//---------------ball
  // open vertex shader
  GLuint ball_vs = loadShader("ball.vert",GL_VERTEX_SHADER);
  // open fragment shader
  GLuint ball_fs = loadShader("ball.frag",GL_FRAGMENT_SHADER);
  // link shaders
  GLuint ball_shaders[2] = {ball_vs,ball_fs};
  GLuint ball_shader_programme = linkShaders(ball_shaders,2);

  int ball_model_mat_location = glGetUniformLocation(ball_shader_programme,"model_mat");
  int ball_view_mat_location = glGetUniformLocation(ball_shader_programme,"view_mat");
  int ball_project_mat_location = glGetUniformLocation(ball_shader_programme,"project_mat");
  int ball_tex_loc = glGetUniformLocation (ball_shader_programme, "ball_texture");
  //int ball_view_ori_mat_location = glGetUniformLocation(ball_shader_programme,"view_ori_mat");
  
//----------------shader for box and bunny
  // open vertex shader
  GLuint box_vs = loadShader("bunny.vert",GL_VERTEX_SHADER);
  // open fragment shader
  GLuint box_fs = loadShader("bunny.frag",GL_FRAGMENT_SHADER);
  // link shaders
  GLuint box_shaders[2] = {box_vs,box_fs};
  GLuint box_shader_programme = linkShaders(box_shaders,2);

  int box_model_mat_location = glGetUniformLocation(box_shader_programme,"model_mat");
  int box_view_mat_location = glGetUniformLocation(box_shader_programme,"view_mat");
  int box_project_mat_location = glGetUniformLocation(box_shader_programme,"project_mat");
  int texture_noise_ramp_control_location = glGetUniformLocation(box_shader_programme,"texture_noise_ramp");
  int box_tex_loc = glGetUniformLocation (box_shader_programme, "box_texture");
  int box_nois_tex_loc = glGetUniformLocation (box_shader_programme, "noise_texture");
  int box_ramp_tex_loc = glGetUniformLocation (box_shader_programme, "ramp_texture");
  int time_loc = glGetUniformLocation( box_shader_programme, "time");

//================================define variables for main loop==================
  float field_of_view = 67.0f;
  float near_plane_z = 0.1;
  float far_plane_z = 100;

  
  float moon_pos[3] = {-2.0f,0.0f,0.0f};
  
  
  float box_pos[3] = {2.0f,0.0f,0.0f};
  

  float bunny_pos[3] = {0.0f,0.0f,0.0f};
  

  float camera_pos[3] = {0.0f , 0.0f , 2.0f };
  // camera_ori is moved to global;

  
  
  double time0 = glfwGetTime();
  double time1 , period;
  float * poses[5] = {0,box_pos,earth_pos,moon_pos,bunny_pos};
  
  glClearColor(0.6f,0.6f,0.8f,1.0f);

  //=================================Main loop=================================
  while (!glfwWindowShouldClose (window)) {
    _update_fps_counter(window);
    time1 = glfwGetTime();
    period = time1-time0;
    time0 = time1;
    
    earth_ori[2] = time1*0.3;
    moon_ori[2] =  time1 * 0.6;
    box_ori[1] = time1 * 0.2;
    box_ori[0] = time1 * 0.3;
    
    //bunny_ori[1] = time1 * 0.2;
    //bunny_ori[2] = time1 *0.4;

    // wipe the drawing surface clear
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport (0, 0, g_fb_width, g_fb_height);
    /*glm::mat4 glm_view_mat = glm::mat4(1.0f);
    glm_view_mat = glm::rotate(glm_view_mat,(-camera_ori[0]),glm::vec3(1.0f,0.0f,0.0f));
    glm_view_mat = glm::rotate(glm_view_mat,(-camera_ori[1]),glm::vec3(0.0f,1.0f,0.0f));
    glm_view_mat = glm::rotate(glm_view_mat,(-camera_ori[2]),glm::vec3(0.0f,0.0f,1.0f));
    glm_view_mat = glm::translate(glm_view_mat,glm::vec3(-camera_pos[0],-camera_pos[1],-camera_pos[2]));

    glm::mat4 glm_proj_mat = glm::frustumRH_NO(-0.1,0.1,-0.1,0.1,0.1,100.0);
    */
    float view_mat[16];
    id4(view_mat);
    translate4(view_mat,-camera_pos[0],-camera_pos[1],-camera_pos[2]);
    rotate4(view_mat,-camera_ori[0],-camera_ori[1],-camera_ori[2]);
    
    float proj_mat[16];
    id4(proj_mat); 
    project4(proj_mat,field_of_view,aspect_ratio,near_plane_z,far_plane_z);

    float camera_ori_mat[16];
    id4(camera_ori_mat);
    rotate4v(camera_ori_mat,camera_ori);

    //fshowmat4(stderr, proj_mat);
    if(show_earth){
      glUseProgram (ball_shader_programme);
      float earth_model_mat[16];
      id4(earth_model_mat);
      rotate4v(earth_model_mat,earth_ori);
      rotate4(earth_model_mat,1.5,0.2,0.0);
      translate4v(earth_model_mat,earth_pos);

      glUniformMatrix4fv(ball_model_mat_location ,1, GL_TRUE,earth_model_mat);
      glUniformMatrix4fv(ball_view_mat_location,1,GL_TRUE,view_mat);		     
      glUniformMatrix4fv(ball_project_mat_location,1,GL_TRUE,proj_mat);
      

      //glUniformMatrix4fv(ball_view_mat_location,1,GL_FALSE,glm::value_ptr(glm_view_mat));		     
      //glUniformMatrix4fv(ball_project_mat_location,1,GL_FALSE,glm::value_ptr(glm_proj_mat));
      //glUniformMatrix4fv(ball_view_ori_mat_location,1,GL_TRUE,view_ori_mat);
      glUniform1i (ball_tex_loc, 1);
      //print_all(ball_shader_programme);
      glBindVertexArray (vao_ball);
      // draw points 0-3 from the currently bound VAO with current in-use shader
      glDrawElements (GL_TRIANGLE_STRIP, (BALL_RES+1)*2*(BALL_RES-1),GL_UNSIGNED_INT,NULL);
      //box shader begin
    }
    if(show_moon){
      glUseProgram (ball_shader_programme);
      float moon_model_mat[16];
      id4(moon_model_mat);
      rotate4v(moon_model_mat,moon_ori);
      rotate4(moon_model_mat,-1.5,0.2,0.0);
      translate4v(moon_model_mat,moon_pos);

      glUniformMatrix4fv(ball_model_mat_location ,1, GL_TRUE,moon_model_mat);
      glUniformMatrix4fv(ball_view_mat_location,1,GL_TRUE,view_mat);		     
      glUniformMatrix4fv(ball_project_mat_location,1,GL_TRUE,proj_mat);
      glUniform1i (ball_tex_loc, 2);
      //print_all(shader_programme);
      glBindVertexArray (vao_ball);
      // draw points 0-3 from the currently bound VAO with current in-use shader
      glDrawElements (GL_TRIANGLE_STRIP, (BALL_RES+1)*2*(BALL_RES-1),GL_UNSIGNED_INT,NULL);
      //box shader begin
    }
    if(show_box){
      glUseProgram (box_shader_programme);
      float box_model_mat[16];
      id4(box_model_mat);
      scale4(box_model_mat,0.5);
      rotate4v(box_model_mat,box_ori);
      translate4v(box_model_mat,box_pos);

      glUniformMatrix4fv(box_model_mat_location ,1, GL_TRUE,box_model_mat );
      glUniformMatrix4fv(box_view_mat_location,1,GL_TRUE,view_mat);		     
      glUniformMatrix4fv(box_project_mat_location,1,GL_TRUE,proj_mat);
      float boxDefaut[3] = {1.0f,0.0f,0.0f};
      glUniform3fv(texture_noise_ramp_control_location,1,boxDefaut);
      glUniform1f(time_loc , time1);
      glUniform1i (box_tex_loc, 0);
      glUniform1i (box_nois_tex_loc , 4);
      glUniform1i (box_ramp_tex_loc , 5);
      //print_all(shader_programme);
      glBindVertexArray (vao_box);
      glDrawArrays(GL_TRIANGLES,0,36);
    }
    if(show_bunny){
      glUseProgram (box_shader_programme);
      float bunny_model_mat[16];
      id4(bunny_model_mat);
      scale4(bunny_model_mat,0.5);
      rotate4v(bunny_model_mat,bunny_ori);
      translate4v(bunny_model_mat,bunny_pos);

      glUniformMatrix4fv(box_model_mat_location ,1, GL_TRUE,bunny_model_mat );
      glUniformMatrix4fv(box_view_mat_location,1,GL_TRUE,view_mat);		     
      glUniformMatrix4fv(box_project_mat_location,1,GL_TRUE,proj_mat);
      glUniform1i (box_tex_loc, 3);
      glUniform3fv(texture_noise_ramp_control_location,1,texture_noise_ramp_control);
      glUniform1f(time_loc , time1);
      glUniform1i (box_nois_tex_loc , 4);
      glUniform1i (box_ramp_tex_loc , 5);
      
      //print_all(shader_programme);
      glBindVertexArray (vao_bunny);
      glDrawArrays(GL_TRIANGLES,0,bunny_vertices.size());
    }

    
    // update other events like input handling
    glfwPollEvents ();
    // put the stuff we've been drawing onto the display
    glfwSwapBuffers (window);

    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_UP)){
      poses[last_shown][1]-= period   ;
    }
    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_DOWN)){
      poses[last_shown][1]+= period  ;
    }
    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_RIGHT)){
      poses[last_shown][0]-= period  ;
    }
    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_LEFT)){
      poses[last_shown][0]+= period  ;
    }
    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_F)){
      poses[last_shown][2]-= period  ;
    }
    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_B)){
      poses[last_shown][2]+= period  ;
    }
    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_PAGE_UP)){
      field_of_view -= period * 20;
    }
    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_PAGE_DOWN)){
      field_of_view += period * 20 ;
    }
  

    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_W)){
      float f[4] = {0.0,0.0,-1.0,0.0};
      matapp4(camera_ori_mat,f);
      camera_pos[0] += period * f[0] * 0.5 ;
      camera_pos[1] += period * f[1] * 0.5 ;
      camera_pos[2] += period * f[2] * 0.5 ;
    }
    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_X)){
      float b[4] ={0.0,0.0,1.0,0.0};
      matapp4(camera_ori_mat,b);
      camera_pos[0] += period * b[0] * 0.5 ;
      camera_pos[1] += period * b[1] * 0.5 ;
      camera_pos[2] += period * b[2] * 0.5 ;
    }
    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_A)){
      float l[4] ={1.0,0.0,0.0,0.0};
      matapp4(camera_ori_mat,l);
      camera_pos[0] += period * l[0] * 0.5 ;
      camera_pos[1] += period * l[1] * 0.5 ;
      camera_pos[2] += period * l[2] * 0.5 ;
    }
    if(GLFW_PRESS == glfwGetKey (window, GLFW_KEY_D)){
      float r[4] ={-1.0,0.0,0.0,0.0};
      matapp4(camera_ori_mat,r);
      camera_pos[0] += period * r[0] * 0.5 ;
      camera_pos[1] += period * r[1] * 0.5 ;
      camera_pos[2] += period * r[2] * 0.5 ;
    }


    if (GLFW_PRESS == glfwGetKey (window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose (window, 1);
    }
    
    if (GLFW_PRESS == glfwGetKey (window, GLFW_KEY_R)) {
      ball_vs = loadShader("ball.vert",GL_VERTEX_SHADER);
      // open fragment shader
      ball_fs = loadShader("ball.frag",GL_FRAGMENT_SHADER);
      // link shaders
      ball_shaders[0] = ball_vs; ball_shaders[1]=ball_fs;
      ball_shader_programme = linkShaders(ball_shaders,2);
      ball_model_mat_location = glGetUniformLocation(ball_shader_programme,"model_mat");    
      ball_view_mat_location = glGetUniformLocation(ball_shader_programme,"view_mat");      
      ball_project_mat_location = glGetUniformLocation(ball_shader_programme,"project_mat");
      ball_tex_loc = glGetUniformLocation (ball_shader_programme, "ball_texture");

      box_vs = loadShader("box.vert",GL_VERTEX_SHADER);
      // open fragment shader
      box_fs = loadShader("box.frag",GL_FRAGMENT_SHADER);
      // link shaders
      box_shaders[0] = box_vs; box_shaders[1]=box_fs;
      box_shader_programme = linkShaders(box_shaders,2);
      box_model_mat_location = glGetUniformLocation(box_shader_programme,"model_mat");    
      box_view_mat_location = glGetUniformLocation(box_shader_programme,"view_mat");      
      box_project_mat_location = glGetUniformLocation(box_shader_programme,"project_mat");
      box_tex_loc = glGetUniformLocation (box_shader_programme, "box_texture");
      
    }
    
  }
  

  // close GL context and any other GLFW resources
  
  glfwTerminate();
  return 0;
}

bool restart_gl_log () {
  FILE* file = fopen (GL_LOG_FILE, "w");
  if (!file) {
    fprintf (
	     stderr,
	     "ERROR: could not open GL_LOG_FILE log file %s for writing\n",
	     GL_LOG_FILE
	     );
    return false;
  }
  time_t now = time (NULL);
  char* date = ctime (&now);
  fprintf (file, "GL_LOG_FILE log. local time %s\n", date);
  fclose (file);
  return true;
}


bool gl_log (const char* message, ...) {
  va_list argptr;
  FILE* file = fopen (GL_LOG_FILE, "a");if (!file) {
    fprintf (
	     stderr,
	     "ERROR: could not open GL_LOG_FILE %s file for appending\n",
	     GL_LOG_FILE
	     );
    return false;
  }
  va_start (argptr, message);
  vfprintf (file, message, argptr);
  va_end (argptr);
  fclose (file);
  return true;
}


bool gl_log_err (const char* message, ...) {
  va_list argptr;
  FILE* file = fopen (GL_LOG_FILE, "a");
  if (!file) {
    fprintf (
	     stderr,
	     "ERROR: could not open GL_LOG_FILE %s file for appending\n",
	     GL_LOG_FILE
	     );
    return false;
  }
  va_start (argptr, message);
  vfprintf (file, message, argptr);
  va_end (argptr);
  va_start (argptr, message);
  vfprintf (stderr, message, argptr);
  va_end (argptr);
  fclose (file);
  return true;
}
void glfw_error_callback (int error, const char* description) {
  gl_log_err ("GLFW ERROR: code %i msg: %s\n", error, description);
}

void log_gl_params () {
  GLenum params[] = {
		     GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		     GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		     GL_MAX_DRAW_BUFFERS,
		     GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
		     GL_MAX_TEXTURE_IMAGE_UNITS,
		     GL_MAX_TEXTURE_SIZE,
		     GL_MAX_VARYING_FLOATS,
		     GL_MAX_VERTEX_ATTRIBS,
		     GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
		     GL_MAX_VERTEX_UNIFORM_COMPONENTS,
		     GL_MAX_VIEWPORT_DIMS,
		     GL_STEREO,
  };
  const char* names[] = {
			 "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
			 "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
			 "GL_MAX_DRAW_BUFFERS",
			 "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
			 "GL_MAX_TEXTURE_IMAGE_UNITS",
			 "GL_MAX_TEXTURE_SIZE",
			 "GL_MAX_VARYING_FLOATS",
			 "GL_MAX_VERTEX_ATTRIBS",
			 "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
			 "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
			 "GL_MAX_VIEWPORT_DIMS",
			 "GL_STEREO",
  };
  gl_log ("GL Context Params:\n");
  // integers - only works if the order is 0-10 integer return types
  for (int i = 0; i < 10; i++) {
    int v = 0;
    glGetIntegerv (params[i], &v);
    gl_log ("%s %i\n", names[i], v);
  }
  // others
  int v[2];
  v[0] = v[1] = 0;
  glGetIntegerv (params[10], v);
  gl_log ("%s %i %i\n", names[10], v[0], v[1]);
  unsigned char s = 0;
  glGetBooleanv (params[11], &s);
  gl_log ("%s %u\n", names[11], (unsigned int)s);
  gl_log ("-----------------------------\n");
}


/* we will use this function to update the window title with a frame rate */
void _update_fps_counter (GLFWwindow* window) {
  double current_seconds;
  double elapsed_seconds;
  current_seconds = glfwGetTime ();
  elapsed_seconds = current_seconds - previous_seconds;
  /* limit text updates to 4 per second */
  if (elapsed_seconds > 0.25) {
    previous_seconds = current_seconds;
    char tmp[128];
    double fps = (double)frame_count / elapsed_seconds;
    sprintf (tmp, "opengl @ fps: %.2f", fps);glfwSetWindowTitle (window, tmp);
    frame_count = 0;
  }
  frame_count++;
}
void _print_shader_info_log (GLuint shader_index) {
  int max_length = 2048;
  int actual_length = 0;
  char log[2048];
  glGetShaderInfoLog (shader_index, max_length, &actual_length, log);
  printf ("shader info log for GL index %u:\n%s\n", shader_index, log);
}
void _print_programme_info_log (GLuint programme) {
  int max_length = 2048;
  int actual_length = 0;
  char log[2048];
  glGetProgramInfoLog (programme, max_length, &actual_length, log);
  printf ("program info log for GL index %u:\n%s", programme, log);
}
void print_all (GLuint programme) {
  printf ("--------------------\nshader programme %i info:\n", programme);
  int params = -1;
  glGetProgramiv (programme, GL_LINK_STATUS, &params);
  printf ("GL_LINK_STATUS = %i\n", params);
  glGetProgramiv (programme, GL_ATTACHED_SHADERS, &params);
  printf ("GL_ATTACHED_SHADERS = %i\n", params);
  glGetProgramiv (programme, GL_ACTIVE_ATTRIBUTES, &params);
  printf ("GL_ACTIVE_ATTRIBUTES = %i\n", params);
  for (GLuint i = 0; i < (GLuint)params; i++) {
    char name[64];
    int max_length = 64;
    int actual_length = 0;
    int size = 0;
    GLenum type;
    glGetActiveAttrib (programme, i, max_length, &actual_length, &size, &type,name);
    if (size > 1) {
      for (int j = 0; j < size; j++) {
	char long_name[64];
	sprintf (long_name, "%s[%i]", name, j);
	int location = glGetAttribLocation (programme, long_name);
	printf (" %i) type:%s name:%s location:%i\n", i, GL_type_to_string (type),long_name, location);
      }
    } else {
      int location = glGetAttribLocation (programme, name);
      printf (" %i) type:%s name:%s location:%i\n", i, GL_type_to_string (type),name, location);
    }
  }
  glGetProgramiv (programme, GL_ACTIVE_UNIFORMS, &params);
  printf ("GL_ACTIVE_UNIFORMS = %i\n", params);
  for (GLuint i = 0; i < (GLuint)params; i++) {
    char name[64];
    int max_length = 64;
    int actual_length = 0;
    int size = 0;
    GLenum type;
    glGetActiveUniform (programme, i, max_length, &actual_length, &size, &type,name);
    if (size > 1) {
      for (int j = 0; j < size; j++) {
	char long_name[64];
	sprintf (long_name, "%s[%i]", name, j);
	int location = glGetUniformLocation (programme, long_name);
	printf (" %i) type:%s name:%s location:%i\n", i, GL_type_to_string (type),long_name, location);
      }
    } else {
      int location = glGetUniformLocation (programme, name);
      printf (" %i) type:%s name:%s location:%i\n", i, GL_type_to_string (type),name, location);
    }
  }
  _print_programme_info_log (programme);
}
const char* GL_type_to_string (GLenum type) {
  switch (type) {case GL_BOOL: return "bool";
  case GL_INT: return "int";
  case GL_FLOAT: return "float";
  case GL_FLOAT_VEC2: return "vec2";
  case GL_FLOAT_VEC3: return "vec3";
  case GL_FLOAT_VEC4: return "vec4";
  case GL_FLOAT_MAT2: return "mat2";
  case GL_FLOAT_MAT3: return "mat3";
  case GL_FLOAT_MAT4: return "mat4";
  case GL_SAMPLER_2D: return "sampler2D";
  case GL_SAMPLER_3D: return "sampler3D";
  case GL_SAMPLER_CUBE: return "samplerCube";
  case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
  default: break;
  }
  return "other";
}

bool is_valid (GLuint programme) {
    glValidateProgram (programme);
    int params = -1;
    glGetProgramiv (programme, GL_VALIDATE_STATUS, &params);
    printf ("program %i GL_VALIDATE_STATUS = %i\n", programme, params);
    if (GL_TRUE != params) {
      _print_programme_info_log (programme);
      return false;
    }
    return true;
}

GLuint loadShader(char* filename,GLenum shader_type){
  // open vertex shader
  FILE * file  = fopen( filename , "r"); // read only
  if (file==NULL) {fprintf(stderr,"cannot open %s",filename); exit (1);}
  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  fseek(file, 0, SEEK_SET);  /* same as rewind(f); */
  char *source = (char *)malloc(fsize + 1);
  fread(source, 1, fsize, file);
  source[fsize] = 0;
  GLuint shader = glCreateShader (shader_type);
  glShaderSource (shader, 1, &source, NULL);
  glCompileShader (shader);
  // check for compile errors
  int params = -1;
  glGetShaderiv (shader, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    fprintf (stderr, "ERROR: GL shader index %i did not compile\n", shader);
    _print_shader_info_log (shader);
    exit(1); // or exit or something
  }
  fclose(file);
  free(source);
  return shader;
}


GLuint linkShaders(GLuint* shaders , unsigned short num){
  GLuint shader_programme = glCreateProgram ();
  for(unsigned short i=0;i<num;i++)
    glAttachShader (shader_programme,shaders[i]);
  glLinkProgram (shader_programme);
  // check if link was successful
  int params = -1;
  glGetProgramiv (shader_programme, GL_LINK_STATUS, &params);
  if (GL_TRUE != params) {
    fprintf (stderr, "ERROR: could not link shader programme GL index %u\n",shader_programme);
    _print_programme_info_log (shader_programme);
    return false;
  }
  //validate shaders
  if(is_valid(shader_programme)) return shader_programme;  
  else {printf("shader_programme not valid !!") ;exit(1);}
}
void loadtexture(char* filename, unsigned short tex_index){
   //load texture images
  int x, y, n;
  int force_channels = 4;
  unsigned char* img = stbi_load (filename, &x, &y, &n, force_channels);
  if (!img) {
    fprintf (stderr, "ERROR: could not load %s\n",filename);
  }
  // NPOT check
  if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
    fprintf (stderr, "WARNING: texture %s is not power-of-2 dimensions\n", filename);
  }
  GLuint tex = 0;
  glGenTextures (1, &tex);
  glActiveTexture (GL_TEXTURE0+tex_index);
  glBindTexture (GL_TEXTURE_2D, tex);
  glTexImage2D (
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		img
		);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  return;
}


