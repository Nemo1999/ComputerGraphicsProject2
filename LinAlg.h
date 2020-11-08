// This is written by nemo for CG project 1
// Contains some linear algebra utilities 


// Matrixes and vectors are represented in "row-major" for visual conveniece
// Remember to use the transpose option to GL_TRUE when sending them into GLuniform.

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#ifndef PI
#define PI 3.14159265358979
#endif

#ifndef _LIN_ALG_CPP_
#define _LIN_ALG_CPP_


typedef float* vec4;

typedef float* mat4;

typedef float* vec3;

typedef float* mat3;

void negate3v(float* in){
  in[0]=-in[0];
  in[1]=-in[1];
  in[2]=-in[2];
}

void id4(mat4 in){
  for(short i=0;i<4;i++)
    for(short j=0;j<4;j++)
      if(i==j) in[i*4+j]=1.0f;else in[i*4+j]=0.0f;
}

void scale4(mat4 in , float scale){
  float in_copy[16];
  for(short i=0;i<16;i++)
    in_copy[i]=in[i];
  float scale_mat[16]= {scale,0.0,0.0,0.0,
		  0.0,scale,0.0,0.0,
		  0.0,0.0,scale,0.0,
		  0.0,0.0,0.0,1.0};
  
  for(short i=0;i<4;i++)
    for(short j=0;j<4;j++){
      float ind = 0;
      for(short k=0;k<4;k++)
	      ind += scale_mat[i*4+k] * in_copy[j+4*k];
      in[i*4+j]=ind;
    }
}

void rotate4(float* in, float wx, float wy, float wz){
  float in_copy[16];
  for(short i=0;i<16;i++)
    in_copy[i]=in[i];
  
  float cx = cos(wx) , cy = cos(wy) , cz = cos(wz);
  float sx = sin(wx) , sy = sin(wy) , sz = sin(wz);
  float rotate_mat[16] = {cz*cy, cz*sy*sx - sz*cx , cz*sy*cx + sz*sx , 0.0,
	      sz*cy, sz*sy*sx + cz*cx , sz*sy*cx - cz*sx , 0.0,
	      -sy  , cy*sx            , cy*cx            , 0.0,
	      0.0  , 0.0              , 0.0              , 1.0
  };
  
  for(short i=0;i<4;i++)
    for(short j=0;j<4;j++){
      float ind = 0;
      for(short k=0;k<4;k++)
	      ind += rotate_mat[i*4+k] * in_copy[j+4*k];
      in[i*4+j]=ind;
    }
}


void rotate4v(float* in,float* orientation){
  rotate4(in,orientation[0],orientation[1],orientation[2]);
}

void translate4(float* in,float dx, float dy, float dz){
  float in_copy[16];
  for(short i=0;i<16;i++)
    in_copy[i]=in[i];
  float trans_mat[16] ={ 1.0 , 0.0 , 0.0 , dx ,
		       0.0 , 1.0 , 0.0 , dy ,
		       0.0 , 0.0 , 1.0 , dz ,
		       0.0 , 0.0 , 0.0 , 1.0  };
  for(short i=0;i<4;i++)
    for(short j=0;j<4;j++){
      float ind = 0;
      for(short k=0;k<4;k++)
	      ind += trans_mat[i*4+k] * in_copy[j+4*k];
      in[i*4+j]=ind;
    }
}

void translate4v(float* in,float* pos){
  translate4(in,pos[0],pos[1],pos[2]);
}


void project4(float* in,float fov, float aspect, float near, float far ){
  float in_copy[16];
  for(short i=0;i<16;i++)
    in_copy[i]=in[i];
  float range = tan((fov/(float)180)* PI * 0.5) * near;
  float Sx = near / (range * aspect);
  float Sy = near / range ;
  float Sz = -(far + near) / (far - near);
  float Pz = -(2 * far * near) / (far - near);
  float proj_mat[16] = {
        -Sx , 0.0,  0.0, 0.0,
		    0.0,  -Sy,  0.0, 0.0,
		    0.0, 0.0,   Sz,  Pz,
		    0.0, 0.0, -1.0, 0.0
		    };
  for(short i=0;i<4;i++)
    for(short j=0;j<4;j++){
      float ind = 0;
      for(short k=0;k<4;k++)
	      ind += proj_mat[i*4+k] * in_copy[j+4*k];
      in[i*4+j]=ind;
    }        
}

void matmult4(mat4 A , mat4 B){
  float in_copy[16];
  for(short i=0;i<16;i++)
    in_copy[i]=A[i];
  for(short i=0;i<4;i++)
    for(short j=0;j<4;j++){
      float ind = 0;
      for(short k=0;k<4;k++)
	      ind += in_copy[i*4+k] * B[j+4*k];
      A[i*4+j]=ind;
    }   

}

void matapp4(mat4 A , vec4 v){
  float v_copy[4];
  for(short i=0;i<4;i++)
    v_copy[i]=v[i];

  for(short i=0;i<4;i++){
    float ind = 0;
    for(short k=0;k<4;k++)
      ind +=A[4*i+k] * v_copy [k];
    v[i] = ind;
  }
}  

void fshowmat4(FILE* file, mat4 A){
  for(short i=0;i<16;i+=4)
    fprintf(file,"\t%3f\t%3f\t%3f\t%3f\n",A[i],A[i+1],A[i+2],A[i+3]);
}

void fshowvec4(FILE* file, vec4 v){
  for(short i=0;i<4;i++)
    fprintf(file,"\t%3f\n",v[i]);
}



#endif
