#ifndef _CAXES_H_
#define _CAXES_H_

#include "vecmaths.h"
#include "cpipe3d.h"


// Struct for a vertex
typedef struct
{
	FVector Vertex;				// Untransformed position
	FVector TVertex;			// Transformed position
	u_char r, g, b, a;			// colour and alpha
	int ADC;					// Clipping bit
}LineVertex_t;



// Line Structure
typedef struct
{
	LineVertex_t P[2];
}Line_t;

class CAxes
{
public:
	CAxes();
	~CAxes();
	
	int BuildGSPacket(void *& pMem, CPipe3d * pPipe3D);
	void SetWorldMatrix(FMatrix World);
	
	
	
	// Line for each axis
	Line_t Line[3];	
		
private:
	float m_Length;
	
	// world positional matrix
	FMatrix m_World;				
};


#endif

