// This is written by nemo for CG project 1
// Contains some linear algebra utilities 


// Matrixes and vectors are represented in "row-major" for visual conveniece
// Remember to use the transpose option when sending them into GLuniform.

// we use pass-by value for simplicity (which enables nested function application).
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#ifndef PI
#define PI 3.14159265358979
#endif

#ifndef _LIN_ALG_
#define _LIN_ALG_


typedef struct vec4{
  float v[4];
} vec4;

typedef struct mat4{
  float m[16];
} mat4;

typedef struct vec3{
  float v[3];
} vec3;

typedef struct mat3{
  float m[9];
} mat3;

vec3 negate3v(float* in){
  vec3 ans = {.v={-in[0],-in[1],-in[2]}};
  return ans;
}


mat4 scale4(float scale){
  mat4 ans = {.m={scale,0.0,0.0,0.0,
		  0.0,scale,0.0,0.0,
		  0.0,0.0,scale,0.0,
		  0.0,0.0,0.0,1.0}};
  return ans;
}

mat4 rotate4(float wx, float wy, float wz){
  float cx = cos(wx) , cy = cos(wy) , cz = cos(wz);
  float sx = sin(wx) , sy = sin(wy) , sz = sin(wz);
  mat4 ans = {cz*cy, cz*sy*sx - sz*cx , cz*sy*cx + sz*sx , 0.0,
	      sz*cy, sz*sy*sx + cz*cx , sz*sy*cx - cz*sx , 0.0,
	      -sy  , cy*sx            , cy*cx            , 0.0,
	      0.0  , 0.0              , 0.0              , 1.0
  };
  return ans;
}


mat4 rotate4v(float* orientation){
  return rotate4(orientation[0],orientation[1],orientation[2]);
}



mat4 translate4(float dx, float dy, float dz){
  mat4 ans =    { .m={ 1.0 , 0.0 , 0.0 , dx ,
		       0.0 , 1.0 , 0.0 , dy ,
		       0.0 , 0.0 , 1.0 , dz ,
		       0.0 , 0.0 , 0.0 , 1.0  }
  };
  return ans;
}

mat4 translate4v(float* pos){
  return translate4(pos[0],pos[1],pos[2]);
}


mat4 project4(float fov, float aspect, float near, float far ){
  float range = tan((fov/(float)180)* PI * 0.5) * near;
  float Sx = near / (range * aspect);
  float Sy = near / range ;
  float Sz = -(far + near) / (far - near);
  float Pz = -(2 * far * near) / (far - near);
  printf("Sx = %f",Sx);
  mat4 ansp = {.m = {
        -Sx , 0.0,  0.0, 0.0,
		    0.0,  -Sy,  0.0, 0.0,
		    0.0, 0.0,   Sz,  Pz,
		    0.0, 0.0, -1.0, 0.0
		    }
  };
  return ansp;
}

mat4 matmult4(mat4 A , mat4 B){
  mat4 ans;
  for(short i=0;i<4;i++)
    for(short j=0;j<4;j++){
      float ind = 0;
      for(short k=0;k<4;k++)
	ind += A.m[i*4+k] * B.m[j+4*k];
      ans.m[i*4+j]=ind;
    }
  return ans;
}

vec4 matapp4(mat4 A , vec4 v){
  vec4 ans;
  for(short i=0;i<4;i++){
    float ind = 0;
    for(short k=0;k<4;k++)
      ind += A.m[4*i+k] * v.v[k];
    ans.v[i] = ind;
  }
  return ans;
}  

void fshowmat4(FILE* file, mat4 A){
  for(short i=0;i<16;i+=4)
    fprintf(file,"\t%3f\t%3f\t%3f\t%3f\n",A.m[i],A.m[i+1],A.m[i+2],A.m[i+3]);
}

void fshowvec4(FILE* file, vec4 v){
  for(short i=0;i<4;i++)
    fprintf(file,"\t%3f\n",v.v[i]);
}



#endif
