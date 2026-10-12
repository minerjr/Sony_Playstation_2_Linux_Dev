// Some basic vextor and matrix maths routines

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
void CopyFVector(FVector * const v0, const FVector * const v1)
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
void IdentityMatrix(FMatrix * const m)
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
void ApplyMatrix(FVector * const v0, const FMatrix * const m0, const FVector * const v1)
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
void SubVector(FVector * const v0, const FVector * const v1, const FVector * const v2)
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
void Normalise(FVector * const v0, const FVector * const v1)
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
void CrossProduct(FVector * const v0, const FVector * const v1, const FVector * const v2)
{
  v0->v[0] = v1->v[1] * v2->v[2] - v1->v[2] * v2->v[1];
  v0->v[1] = v1->v[2] * v2->v[0] - v1->v[0] * v2->v[2];
  v0->v[2] = v1->v[0] * v2->v[1] - v1->v[1] * v2->v[0];
  v0->v[3] = 1.0f;
}


// ==========================================================================
//  	float DotProduct(FVECTOR v0, FVECTOR v1)
// --------------------------------------------------------------------------
float DotProduct(const FVector * const v0, const FVector * const v1)
{
  return (v0->v[0]*v1->v[0] + v0->v[1]*v1->v[1] + v0->v[2]*v1->v[2]);
}



// ==========================================================================
//  	void MulMatrix(FMATRIX m0, FMATRIX m1, FMATRIX m2)
//  		m0=m1*m2
// --------------------------------------------------------------------------
void MulMatrix(FMatrix * const m0, const FMatrix * const m1, const FMatrix * const m2)
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
void CopyMatrix(FMatrix * const m0, const FMatrix * const m1)
{
  int i,j;
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      m0->m[i][j] = m1->m[i][j];
    }
  }
}

// output vector contents
void DumpFVector(char * s, const FVector * const v)
{
	printf(s);
	printf("\t\t[%f,\t%f,\t%f,\t%f]\n\n",  v->v[0], v->v[1], v->v[2], v->v[3]);
}

// output vector contents
void DumpIVector(char *s, const IVector * const v)
{
	printf(s);
	printf("\t\t[%d,\t%d,\t%d,\t%d]\n\n",  v->v[0], v->v[1], v->v[2], v->v[3]);
}

// output matrix contents
void DumpMatrix(char *s, const FMatrix * const m)
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
void RotMatrixY(FMatrix * const m0, const FMatrix * const m1, float ry)
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
void RotMatrixX(FMatrix * const m0, const FMatrix * const m1, float rx)
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
void RotMatrixZ(FMatrix * const m0, const FMatrix * const m1, float rz)
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
void RotMatrix(FMatrix * const m0, const FMatrix * const m1, float rz, float ry, float rx)
{
  RotMatrixZ(m0, m1, rz);
  RotMatrixY(m0, m0, ry);
  RotMatrixX(m0, m0, rx);
}


void ScaleMatrix(FMatrix * const m0, const FMatrix * const m1, float Scl)
{
	FMatrix Scale;
    
  	IdentityMatrix(&Scale);
  	
  	Scale.m[0][0] = Scl; 
  	Scale.m[1][1] = Scl;
  	Scale.m[2][2] = Scl; 

  	MulMatrix(m0, &Scale, m1);
}

// ==========================================================================
// void TranslateMatrix(FMatrix * m0, FMatrix * m1, float Tx, float Tx, float Tz)
// Input m1, Tx, Ty, Tz
// Output m0
// Translate matrix m1 by amounts Tx, Ty, Tz and output result in m0
// --------------------------------------------------------------------------
void TranslateMatrix(FMatrix * const m0, const FMatrix * const m1, float Tx, float Ty, float Tz)
{
	FMatrix Trans;
    
  	IdentityMatrix(&Trans);
  	
  	Trans.m[0][3] = Tx; 
  	Trans.m[1][3] = Ty;
  	Trans.m[2][3] = Tz; 

  	MulMatrix(m0, &Trans, m1);
}

// Set a vector to the specified values
void SetVector(FVector * const v, float x, float y, float z, float w)
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
