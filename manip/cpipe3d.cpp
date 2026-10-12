#include "sps2.h"
#include "cpipe3d.h"
#include <stdio.h>

CPipe3d::CPipe3d(void)
{
	SetVector(&m_Position, 0.0f, 0.0f, 150.0f, 1.0f);
	SetVector(&m_LookAt, 0.0f, 0.0f, 0.0f, 1.0f);
	SetVector(&m_Up, 0.0f, 1.0f, 0.0f, 1.0f);
	CalcCameraMatrix();
	
	m_Near = -5.0f;
	m_Far = -1000.0f;
	m_Aspect = 4.0f/3.0f;
	m_FOV = 60.0f;
	SetupVS();
	
	m_Width = 640.0f;
	m_Height = 480.0f;
	m_XOffset = 2048.0f;
	m_YOffset = 2048.0f;
	m_Zbuf = (float)(1<<24);
	SetupVT();
	
	CalcWSMatrix();
}


CPipe3d::~CPipe3d(void)
{
}


// Return true to clip, false to draw
bool CPipe3d::ClipCheck(FVector * V)
{
	float Band;
	Band = 1.1f * V->v[3];
	
	if((V->v[0] > Band)||(V->v[0] < -Band)) return true;
	if((V->v[1] > Band)||(V->v[1] < -Band)) return true;
	if((V->v[2] > V->v[3])||(V->v[2] < -V->v[3])) return true;
	
	return false;
}

// Configure the viewport transfromation matrix
void CPipe3d::SetupVT(float Width, float Height)
{
	m_Width = Width;
	m_Height = Height;
	
	SetupVT();
}

// Configure the viewport transfromation matrix
void CPipe3d::SetupVT(float Width, float Height, float XOffset, float YOffset, float Zbuf)
{
	m_Width = Width;
	m_Height = Height;
	m_XOffset = XOffset;
	m_YOffset = YOffset;
	m_Zbuf = Zbuf;
	
	SetupVT();
}


// Configure the viewport transfromation matrix
void CPipe3d::SetupVT(void)
{
	IdentityMatrix(&VT);
	
	VT.m[0][0] = m_Width * 0.5f * 16.0f;
	VT.m[1][1] = m_Height * 0.5f * 16.0f;
	VT.m[2][2] = m_Zbuf * 0.5f;

	VT.m[0][3] = m_XOffset * 16.0f;
	VT.m[1][3] = m_YOffset * 16.0f;
	VT.m[2][3] = m_Zbuf * 0.5f;

	FILE *tmpFile = fopen("View.txt", "w");

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0 ; x<4 ;x++)
		{
			fprintf(tmpFile,"%20.8f",VT.m[x][y]);
		}
		fprintf(tmpFile,"\n");
	}
	fclose(tmpFile);

}


// Divide x, y, and z by w
void CPipe3d::HomoDivide(FVector * V)
{
	V->v[0] /= V->v[3];
	V->v[1] /= V->v[3];
	V->v[2] /= V->v[3];
	V->v[3]  = 1.0f;
}

// Configure the View to Screen matrix
void CPipe3d::SetupVS(float Near, float Far, float Aspect, float FOV  )
{
	m_Near = Near;
	m_Far = Far;
	m_Aspect = Aspect;
	m_FOV = FOV;
	
	SetupVS();
}

// Configure the View to Screen matrix
void CPipe3d::SetupVS(void)
{
	float FovYdiv2 = DegToRad(m_FOV) * 0.5f;
	float TanFOV = sinf(FovYdiv2) / cosf(FovYdiv2);

	IdentityMatrix(&VS);

	VS.m[0][0] = 1.0f/(m_Aspect * TanFOV);
	VS.m[1][1] = -1.0f/TanFOV;
	VS.m[2][2] = (m_Far + m_Near) / (m_Far - m_Near);
	VS.m[2][3] = -(2 * m_Far * m_Near) / (m_Far - m_Near);
	VS.m[3][2] = -1.0f;
	VS.m[3][3] = 0.0f;


	FILE *tmpFile = fopen("Projection.txt", "w");

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0 ; x<4 ;x++)
		{
			fprintf(tmpFile,"%20.8f",VS.m[x][y]);
		}
		fprintf(tmpFile,"\n");
	}
	fclose(tmpFile);


}


void CPipe3d::CalcCameraMatrix(FVector Position, FVector LookAt, FVector Up)
{
	m_Position = Position;
	m_LookAt = LookAt;
	m_Up = Up;

	CalcCameraMatrix();	
}

// Calculate the camera matrix
void CPipe3d::CalcCameraMatrix(void)
{
	// Get the z basis vector, which points straight ahead; the
    // difference from the eye point to the look-at point. This is the 
    // direction of the gaze (+z).
    SubVector(&m_ZAxis, &m_Position, &m_LookAt);
    
	// Normalize the z basis vector.
	Normalise(&m_ZAxis, &m_ZAxis);
	
	// Compute the orthogonal axes from the cross product of the gaze 
	// and the Up vector.
	CrossProduct(&m_XAxis, &m_Up, &m_ZAxis);
	Normalise(&m_XAxis, &m_XAxis);

	// Compute the camera up vector from the coss product of the
	// camera Z and X axes
	CrossProduct(&m_YAxis, &m_ZAxis, &m_XAxis);
	
	//DumpFVector("XAxis", &XAxis);
	//DumpFVector("YAxis", &YAxis);
	//DumpFVector("ZAxis", &ZAxis);
	
	BuildCameraMatrix();
	
    
}

void CPipe3d::BuildCameraMatrix(void)
{
	
	// Start building the matrix. The first three rows contain the 
    // basis vectors used to rotate the view to point at the look-at 
    // point. The fourth row contains the translation values. 
    WC.m[0][0] = m_XAxis.v[0];
    WC.m[0][1] = m_XAxis.v[1];
    WC.m[0][2] = m_XAxis.v[2];
    WC.m[0][3] = -DotProduct(&m_XAxis, &m_Position);

    WC.m[1][0] = m_YAxis.v[0];
    WC.m[1][1] = m_YAxis.v[1];
    WC.m[1][2] = m_YAxis.v[2];
    WC.m[1][3] = -DotProduct(&m_YAxis, &m_Position);

    WC.m[2][0] = m_ZAxis.v[0];
    WC.m[2][1] = m_ZAxis.v[1];
    WC.m[2][2] = m_ZAxis.v[2];
    WC.m[2][3] = -DotProduct(&m_ZAxis, &m_Position);


    WC.m[3][0] = 0.0f;
    WC.m[3][1] = 0.0f;
    WC.m[3][2] = 0.0f;
    WC.m[3][3] = 1.0f;

	FILE *tmpFile = fopen("Camera.txt", "w");

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0 ; x<4 ;x++)
		{
			fprintf(tmpFile,"%20.8f",WC.m[x][y]);
		}
		fprintf(tmpFile,"\n");
	}
	fclose(tmpFile);
}





void CPipe3d::CalcWSMatrix(void)
{
	MulMatrix(&WS, &VS, &WC);
}

void CPipe3d::MoveLX(float Delta)
{
	//this moves the Camera along its local axis by an amount Delta
	m_Position.v[0] += m_XAxis.v[0]* Delta;
	m_Position.v[1] += m_XAxis.v[1]* Delta;
	m_Position.v[2] += m_XAxis.v[2]* Delta;
	
	BuildCameraMatrix();
}

void CPipe3d::MoveLY(float Delta)
{
	//this moves the Camera along its local axis by an amount Delta
	m_Position.v[0] += m_YAxis.v[0]* Delta;
	m_Position.v[1] += m_YAxis.v[1]* Delta;
	m_Position.v[2] += m_YAxis.v[2]* Delta;
	
	BuildCameraMatrix();
}

void CPipe3d::MoveLZ(float Delta)
{
	//this moves the Camera along its local axis by an amount Delta
	m_Position.v[0] += m_ZAxis.v[0]* Delta;
	m_Position.v[1] += m_ZAxis.v[1]* Delta;
	m_Position.v[2] += m_ZAxis.v[2]* Delta;
	
	BuildCameraMatrix();
}

void CPipe3d::RotY(float Delta)
{
	FMatrix Rot;
	
	IdentityMatrix(&Rot);
	RotMatrixY(&Rot, &Rot, Delta);
	ApplyMatrix(&m_XAxis, &Rot, &m_XAxis);
	ApplyMatrix(&m_ZAxis, &Rot, &m_ZAxis);
	Orthogonalise();
	BuildCameraMatrix();
}

void CPipe3d::RotX(float Delta)
{
	FMatrix Rot;
	
	IdentityMatrix(&Rot);
	RotMatrixX(&Rot, &Rot, Delta);
	ApplyMatrix(&m_YAxis, &Rot, &m_YAxis);
	ApplyMatrix(&m_ZAxis, &Rot, &m_ZAxis);
	Orthogonalise();
	BuildCameraMatrix();
}

void CPipe3d::RotZ(float Delta)
{
	FMatrix Rot;
	
	IdentityMatrix(&Rot);
	RotMatrixZ(&Rot, &Rot, Delta);
	ApplyMatrix(&m_YAxis, &Rot, &m_YAxis);
	ApplyMatrix(&m_XAxis, &Rot, &m_XAxis);
	Orthogonalise();
	BuildCameraMatrix();
}


void CPipe3d::Orthogonalise(void)
{
	CrossProduct(&m_XAxis, &m_YAxis, &m_ZAxis);
	Normalise(&m_XAxis, &m_XAxis);
	Normalise(&m_ZAxis, &m_ZAxis);
	CrossProduct(&m_YAxis, &m_ZAxis, &m_XAxis);
	//BuildCameraMatrix();
}
