// 
// "vecmaths.h"
// 
// 
#ifndef _VECMATHS_H_
#define _VECMATHS_H_

#ifdef __cplusplus
extern "C" {
#endif

// ===========================================================================
//              Matrix definition 
//
//     |m[0][0] m[0][1] m[0][2] m[0][3]|
//  m =|m[1][0] m[1][1] m[1][2] m[1][3]|
//     |m[2][0] m[2][1] m[2][2] m[2][3]|
//     |m[3][0] m[3][1] m[3][2] m[3][3]|
// 
// ===========================================================================


// ===========================================================================
//              Vector definition 
//
//     |v[0]|
//  v =|v[1]|
//     |v[2]|
//     |v[3]|
// 
// ===========================================================================


typedef struct
{
	float m[4][4];
} FMatrix;

typedef struct
{
	int m[4][4];
} IMatrix;

typedef struct
{
	float v[4];
} FVector;

typedef struct
{
	int v[4];
} IVector;

/*
typedef struct
{
	FVector Camera;
	FVector LookAt;
	FVector CameraUp;
	FMatrix WS;
}Camera;


*/
// prototypes


void CopyFVector(FVector * v0, FVector * v1);
void SubVector(FVector * v0, FVector * v1, FVector * v2);
void Normalise(FVector * v0, FVector * v1);
void CrossProduct(FVector * v0, FVector * v1, FVector * v2);
float DotProduct(FVector * v0, FVector * v1);
void DumpFVector(char *s, FVector * v);
void DumpIVector(char *s, IVector * v);

void IdentityMatrix(FMatrix * m);
void CopyMatrix(FMatrix * m0, FMatrix * m1);
void ApplyMatrix(FVector * v0, FMatrix * m0, FVector * v1);
void MulMatrix(FMatrix * m0, FMatrix * m1, FMatrix * m2);
void RotMatrixX(FMatrix * m0, FMatrix * m1, float rx);
void RotMatrixY(FMatrix * m0, FMatrix * m1, float ry);
void RotMatrixZ(FMatrix * m0, FMatrix * m1, float rz);
void RotMatrix(FMatrix * m0, FMatrix * m1, float rz, float ry, float rx);
void DumpMatrix(char * s, FMatrix * m);
void ScaleMatrix(FMatrix * m0, FMatrix * m1, float Scl);
void TranslateMatrix(FMatrix * m0, FMatrix * m1, float Tx, float Ty, float Tz);
float DegToRad(float Deg);
void SetVector(FVector  * v, float x, float y, float z, float w);

#ifdef __cplusplus
};
#endif



#endif // _VECMATHS_H_
