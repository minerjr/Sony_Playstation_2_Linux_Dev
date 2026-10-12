// 
// "vecmath.c"
// 
//

#define PS2Maths

#include <math.h>
#include <stdio.h>
#include "vecmaths.h"

#ifdef PS2Maths
	#include "PS2Maths.h"
#endif

#ifdef PS2Maths

	// PS2 Maths functions
	#define _cos_(x) (Cos(x))
	#define _sin_(x) (Sin(x))
	#define _sqrt_(x) (Sqrt(x))

#else

	// Maths.h functions
	#define _cos_(x) (cosf(x))
	#define _sin_(x) (sinf(x))
	#define _sqrt_(x) (sqrtf(x))

#endif




// ==========================================================================
// void CopyFVector(FVECTOR v0, FVECTOR v1)
// Input: v1
// Output v0
// vector v1 is copied unchanged to vector v0
// --------------------------------------------------------------------------
void CopyFVector(FVector * v0, FVector * v1)
{
  v0->v[0] = v1->v[0];
  v0->v[1] = v1->v[1];
  v0->v[2] = v1->v[2];
  v0->v[3] = v1->v[3];
}


// ==========================================================================
// void IdentityMatrix(FMATRIX m0);
// Input: m 
// Output: m
// Identity matrix is produced
// --------------------------------------------------------------------------
void IdentityMatrix(FMatrix * m)
{
  int i,j;
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      m->m[i][j] = (i==j)? 1.0f : 0.0f;
    }
  }
}


// ==========================================================================
// void ApplyMatrix(FVECTOR v0, FMATRIX m0, FVECTOR v1)
// Input: m0, v1
// Output v0
// v0=m0*v1
// vector v1 is multiplied by matrix m0
// --------------------------------------------------------------------------
void ApplyMatrix(FVector * v0, FMatrix * m0, FVector * v1)
{
  FVector v;
  v.v[0] = v1->v[0]*m0->m[0][0] + v1->v[1]*m0->m[0][1] + v1->v[2]*m0->m[0][2] + v1->v[3]*m0->m[0][3];
  v.v[1] = v1->v[0]*m0->m[1][0] + v1->v[1]*m0->m[1][1] + v1->v[2]*m0->m[1][2] + v1->v[3]*m0->m[1][3];
  v.v[2] = v1->v[0]*m0->m[2][0] + v1->v[1]*m0->m[2][1] + v1->v[2]*m0->m[2][2] + v1->v[3]*m0->m[2][3];
  v.v[3] = v1->v[0]*m0->m[3][0] + v1->v[1]*m0->m[3][1] + v1->v[2]*m0->m[3][2] + v1->v[3]*m0->m[3][3];
  CopyFVector(v0, &v);
}


// ==========================================================================
// 	void SubVector(VECTOR v0, FVECTOR v1, FVECTOR v2)
// Input: v1, v2
// Output v0
// v0 = v1 - v2
// --------------------------------------------------------------------------
void SubVector(FVector * v0, FVector * v1, FVector * v2)
{
  v0->v[0] = v1->v[0] - v2->v[0];
  v0->v[1] = v1->v[1] - v2->v[1];
  v0->v[2] = v1->v[2] - v2->v[2];
  v0->v[3] = v1->v[3] - v2->v[3];
}


// ==========================================================================
// void Normalise(FVECTOR v0, FVECTOR v1)
// Input v1
// Output v0
// v0 is normalised version of v1
// --------------------------------------------------------------------------
void Normalise(FVector * v0, FVector * v1)
{
  	float norm, dnorm;
  	norm = _sqrt_( v1->v[0] * v1->v[0] + v1->v[1] * v1->v[1] + v1->v[2] * v1->v[2] );
  	if(norm > 0.0f)
  	{
    	dnorm = 1.0f/norm;
    	v0->v[0] = v1->v[0]*dnorm;
    	v0->v[1] = v1->v[1]*dnorm;
    	v0->v[2] = v1->v[2]*dnorm;
    	v0->v[3] = v1->v[3];
  	}
}


// ==========================================================================
//  	void CrossProduct(FVECTOR v0, FVECTOR v1, FVECTOR v2)
//
// --------------------------------------------------------------------------
void CrossProduct(FVector * v0, FVector * v1, FVector * v2)
{
  v0->v[0] = v1->v[1] * v2->v[2] - v1->v[2] * v2->v[1];
  v0->v[1] = v1->v[2] * v2->v[0] - v1->v[0] * v2->v[2];
  v0->v[2] = v1->v[0] * v2->v[1] - v1->v[1] * v2->v[0];
  v0->v[3] = 1.0f;
}


// ==========================================================================
//  	float DotProduct(FVECTOR v0, FVECTOR v1)
// --------------------------------------------------------------------------
float DotProduct(FVector * v0, FVector * v1)
{
  return (v0->v[0]*v1->v[0] + v0->v[1]*v1->v[1] + v0->v[2]*v1->v[2]);
}



// ==========================================================================
//  	void MulMatrix(FMATRIX m0, FMATRIX m1, FMATRIX m2)
//  		m0=m1*m2
// --------------------------------------------------------------------------
void MulMatrix(FMatrix * m0, FMatrix * m1, FMatrix * m2)
{

  int i, j;
  FMatrix m;
   for(i=0; i<4; i++){
     for(j=0; j<4; j++){
       m.m[i][j] = m1->m[i][0] * m2->m[0][j] + m1->m[i][1] * m2->m[1][j]
	 + m1->m[i][2] * m2->m[2][j] + m1->m[i][3] * m2->m[3][j];
     }
   }
   CopyMatrix(m0, &m);
}

// ==========================================================================
// 	void CopyMatrix(FMATRIX m0, FMATRIX m1);
// 
// --------------------------------------------------------------------------
void CopyMatrix(FMatrix * m0, FMatrix * m1)
{
  int i,j;
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      m0->m[i][j] = m1->m[i][j];
    }
  }
}

// output vector contents
void DumpFVector(char *s, FVector * v)
{
	printf(s);
	printf("\t\t[%f,\t%f,\t%f,\t%f]\n\n",  v->v[0], v->v[1], v->v[2], v->v[3]);
}

// output vector contents
void DumpIVector(char *s, IVector * v)
{
	printf(s);
	printf("\t\t[%d,\t%d,\t%d,\t%d]\n\n",  v->v[0], v->v[1], v->v[2], v->v[3]);
}

// output matrix contents
void DumpMatrix(char *s, FMatrix * m)
{
	printf(s);
	printf("\t\t[%f,\t%f,\t%f,\t%f]\n",   m->m[0][0], m->m[0][1], m->m[0][2], m->m[0][3]);
	printf("\t\t[%f,\t%f,\t%f,\t%f]\n",   m->m[1][0], m->m[1][1], m->m[1][2], m->m[1][3]);
	printf("\t\t[%f,\t%f,\t%f,\t%f]\n",   m->m[2][0], m->m[2][1], m->m[2][2], m->m[2][3]);
	printf("\t\t[%f,\t%f,\t%f,\t%f]\n\n", m->m[3][0], m->m[3][1], m->m[3][2], m->m[3][3]);	

}


// ==========================================================================
// 	void RoyMatrixY(FMATRIX m0, FMATRIX m1, float ry);
// --------------------------------------------------------------------------
void RotMatrixY(FMatrix * m0, FMatrix * m1, float ry)
{
	float cost, sint;	
  	FMatrix rot;

  	cost = _cos_(ry);
  	sint = _sin_(ry);  	
  	
  	IdentityMatrix(&rot);

	rot.m[0][0] = cost; 
  	rot.m[2][0] = sint;
  	rot.m[0][2] = -sint; 
  	rot.m[2][2] = cost;
  	MulMatrix(m0, &rot, m1);
}

// ==========================================================================
// 	void RotMatrixX(MATRIX m0, FMATRIX m1, float ry);
// --------------------------------------------------------------------------
void RotMatrixX(FMatrix * m0, FMatrix * m1, float rx)
{
	float cost, sint;	
  	FMatrix rot;
  	
  	cost = _cos_(rx);
  	sint = _sin_(rx);
  	
  	IdentityMatrix(&rot);
  
  	rot.m[1][1] = cost; 
  	rot.m[1][2] = -sint;
  	rot.m[2][1] = sint; 
  	rot.m[2][2] = cost;
  	MulMatrix(m0, &rot, m1);
}

// ==========================================================================
// 	void RotMatrixZ(FMatrix m0, FMatrix m1, float rz);
// --------------------------------------------------------------------------
void RotMatrixZ(FMatrix * m0, FMatrix * m1, float rz)
{
	float cost, sint;	
  	FMatrix rot;
  
  	cost = _cos_(rz);
  	sint = _sin_(rz);
  
  	IdentityMatrix(&rot);
  	rot.m[0][0] = cost; 
  	rot.m[0][1] = -sint;
  	rot.m[1][0] = sint; 
  	rot.m[1][1] = cost;
  	MulMatrix(m0, &rot, m1);
}

// ==========================================================================
// 	void RotMatrix(FMatrix m0, FMatrix m1, float rz, float ry, float rz);
// --------------------------------------------------------------------------
void RotMatrix(FMatrix * m0, FMatrix * m1, float rz, float ry, float rx)
{
  RotMatrixZ(m0, m1, rz);
  RotMatrixY(m0, m0, ry);
  RotMatrixX(m0, m0, rx);
}


void ScaleMatrix(FMatrix * m0, FMatrix * m1, float Scl)
{
	FMatrix Scale;
    
  	IdentityMatrix(&Scale);
  	
  	Scale.m[0][0] = Scl; 
  	Scale.m[1][1] = Scl;
  	Scale.m[2][2] = Scl; 

  	MulMatrix(m0, &Scale, m1);
}


// Calculate the camera matrix
void CalcCameraMatrix(Camera_t * C)
{
	FVector XAxis, YAxis, ZAxis;
	
    // Get the z basis vector, which points straight ahead; the
    // difference from the eye point to the look-at point. This is the 
    // direction of the gaze (+z).
    SubVector(&ZAxis, &C->Position, &C->LookAt);
    
	// Normalize the z basis vector.
	Normalise(&ZAxis, &ZAxis);
	
	// Compute the orthogonal axes from the cross product of the gaze 
	// and the Up vector.
	CrossProduct(&XAxis, &C->Up, &ZAxis);
	Normalise(&XAxis, &XAxis);

	// Compute the camera up vector from the coss product of the
	// camera Z and X axes
	CrossProduct(&YAxis, &ZAxis, &XAxis);
	
	//DumpFVector("XAxis", &XAxis);
	//DumpFVector("YAxis", &YAxis);
	//DumpFVector("ZAxis", &ZAxis);
	
    // Start building the matrix. The first three rows contain the 
    // basis vectors used to rotate the view to point at the look-at 
    // point. The fourth row contains the translation values. 
    C->WC.m[0][0] = XAxis.v[0];
    C->WC.m[0][1] = XAxis.v[1];
    C->WC.m[0][2] = XAxis.v[2];
    C->WC.m[0][3] = -DotProduct(&XAxis, &C->Position);


    C->WC.m[1][0] = YAxis.v[0];
    C->WC.m[1][1] = YAxis.v[1];
    C->WC.m[1][2] = YAxis.v[2];
    C->WC.m[1][3] = -DotProduct(&YAxis, &C->Position);


    C->WC.m[2][0] = ZAxis.v[0];
    C->WC.m[2][1] = ZAxis.v[1];
    C->WC.m[2][2] = ZAxis.v[2];
    C->WC.m[2][3] = -DotProduct(&ZAxis, &C->Position);


    C->WC.m[3][0] = 0.0f;
    C->WC.m[3][1] = 0.0f;
    C->WC.m[3][2] = 0.0f;
    C->WC.m[3][3] = 1.0f;
}

// Return true to clip, false to draw
bool ClipCheck(FVector * V)
{
	if((V->v[0] > V->v[3])||(V->v[0] < -V->v[3])) return true;
	if((V->v[1] > V->v[3])||(V->v[1] < -V->v[3])) return true;
	if((V->v[2] > V->v[3])||(V->v[2] < -V->v[3])) return true;
	
	return false;
}


// Divide x, y, and z by w
void HomoDivide(FVector * V)
{
	V->v[0] /= V->v[3];
	V->v[1] /= V->v[3];
	V->v[2] /= V->v[3];
	V->v[3]  = 1.0f;
}


// Configure the viewport transfromation matrix
void SetupVT(FMatrix * M, float Width, float Height, float XOffset, float YOffset, float Zbuf)
{
	IdentityMatrix(M);
	
	M->m[0][0] = Width * 0.5f * 16.0f;
	M->m[1][1] = Height * 0.5f * 16.0f;
	M->m[2][2] = Zbuf * 0.5f;

	M->m[0][3] = XOffset * 16.0f;
	M->m[1][3] = YOffset * 16.0f;
	M->m[2][3] = Zbuf * 0.5f;
}

// Configure the View to Screen matrix
void SetupVS(FMatrix * M, float Near, float Far, float Aspect, float FOV  )
{
	float FovYdiv2 = DegToRad(FOV) * 0.5f;
	float TanFOV = sinf(FovYdiv2) / cosf(FovYdiv2);

	IdentityMatrix(M);

	M->m[0][0] = 1.0f/(Aspect * TanFOV);
	M->m[1][1] = 1.0f/TanFOV;
	M->m[2][2] = (Far+Near) / (Far-Near);
	M->m[2][3] = -(2*Far*Near) / (Far-Near);
	M->m[3][2] = -1.0f;
	M->m[3][3] = 0.0f;
}

// Set a vector to the specified values
void SetVector(FVector  * v, float x, float y, float z, float w)
{
	v->v[0] = x;
	v->v[1] = y;
	v->v[2] = z;
	v->v[3] = w;
}

// Convert from degrees to radians
float DegToRad(float Deg)
{
	return (Deg / 180.0f) * PI;
}
