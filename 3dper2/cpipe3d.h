#ifndef _CPIPE3D_H_
#define _CPIPE3D_H_

#include "vecmaths.h"


class CPipe3d
{
public:
	CPipe3d();
	~CPipe3d();
	
	void CalcCameraMatrix(FVector Position, FVector LookAt, FVector Up);
	void SetupVS(float Near, float Far, float Aspect, float FOV);
	void SetupVT(float Width, float Height, float XOffset, float YOffset, float Zbuf);
	void HomoDivide(FVector * V);
	bool ClipCheck(FVector * V);
		
	FMatrix WC;
	FMatrix VS;
	FMatrix VT;
	
private:

	void SetupVS(void);
	void SetupVT(void);
	void CalcCameraMatrix(void);

	FVector m_Position;
	FVector m_LookAt;
	FVector m_Up;
	
	float m_Near;
	float m_Far;
	float m_Aspect;
	float m_FOV;
	
	float m_Width;
	float m_Height;
	float m_XOffset;
	float m_YOffset;
	float m_Zbuf;
};



#endif

