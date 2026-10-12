// 
// "vecmaths.h"
// 
// 
#ifndef _VECMATHS_H_
#define _VECMATHS_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

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


// prototypes
void CopyFVector(FVector * const v0, const FVector * const v1);
void SubVector(FVector * const v0, const FVector * const v1, const FVector * const v2);
void Normalise(FVector * const v0, const FVector * const v1);
void CrossProduct(FVector * const v0, const FVector * const v1, const FVector * const v2);
float DotProduct(const FVector * const v0, const FVector * const v1);
void DumpFVector(char *s, const FVector * const v);
void DumpIVector(char *s, const IVector * const v);
void IdentityMatrix(FMatrix * const m);
void CopyMatrix(FMatrix * const m0, const FMatrix * const m1);
void ApplyMatrix(FVector * const v0, const FMatrix * const m0, const FVector * const v1);
void MulMatrix(FMatrix * const m0, const FMatrix * const m1, const FMatrix * const m2);
void RotMatrixX(FMatrix * const m0, const FMatrix * const m1, float rx);
void RotMatrixY(FMatrix * const m0, const FMatrix * const m1, float ry);
void RotMatrixZ(FMatrix * const m0, const FMatrix * const m1, float rz);
void RotMatrix(FMatrix * const m0, const FMatrix * const m1, float rz, float ry, float rx);
void DumpMatrix(char * s, const FMatrix * const m);
void ScaleMatrix(FMatrix * const m0, const FMatrix * const m1, float Scl);
void TranslateMatrix(FMatrix * const m0, const FMatrix * const m1, float Tx, float Ty, float Tz);
float DegToRad(float Deg);
void SetVector(FVector  * const v, float x, float y, float z, float w);

//#ifdef __cplusplus
//};
//#endif



#endif 
