#ifndef _CPIPE3D_H_
#define _CPIPE3D_H_

#include "vecmaths.h"


class CPipe3d
{
public:
	CPipe3d();
	~CPipe3d();
	
	void CalcCameraMatrix(FVector Position, FVector LookAt, FVector Up);
	void CalcCameraMatrix(void);
	void BuildCameraMatrix(void);
	void SetupVS(float Near, float Far, float Aspect, float FOV);
	void SetupVT(float Width, float Height, float XOffset, float YOffset, float Zbuf);
	void SetupVT(float Width, float Height);
	void HomoDivide(FVector * V);
	bool ClipCheck(FVector * V);
	
	void MoveLX(float Delta);
	void MoveLY(float Delta);
	void MoveLZ(float Delta);
	
	void RotX(float Delta);
	void RotY(float Delta);
	void RotZ(float Delta);	
	
	void Orthogonalise(void);
	
	FMatrix WC;
	FMatrix VS;
	FMatrix VT;
	FMatrix WS;
	
private:

	void SetupVS(void);
	void SetupVT(void);
	void CalcWSMatrix(void);

	FVector m_Position;
	FVector m_LookAt;
	FVector m_Up;
	FVector m_XAxis;
	FVector m_YAxis;
	FVector m_ZAxis;
	
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

