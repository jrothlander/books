/***************************************************************
* CPatchApplication.h                                          *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* serve as a base for all the samples.  For this file to       *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

#define D3DFVF_GENERALVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct GENERAL_VERTEX
{
	float x, y, z;
	DWORD color;
};

#define D3DFVF_PATCHVERTEX (D3DFVF_XYZ)

struct PATCH_VERTEX
{
	float x, y, z;
};

class CPatchApplication : public CHostApplication  
{
public:
	HRESULT SetupDevice();
	CPatchApplication();
	virtual ~CPatchApplication();

	BOOL FillGridBuffer();

	virtual BOOL HandleMessage(MSG *pMessage);
	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PostReset();
	virtual void Render();

	D3DXMATRIX              m_Projection;
	LPDIRECT3DVERTEXBUFFER8 m_pGridVertices;

	float m_ViewerHeight;
	float m_ViewerAngle;
};
