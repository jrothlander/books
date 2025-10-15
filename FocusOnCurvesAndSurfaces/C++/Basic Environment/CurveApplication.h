/***************************************************************
* CCurveApplication.h                                          *
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

//This is out here so that is reusable to the child classes
#define D3DFVF_GENERALVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct GENERAL_VERTEX
{
	float x, y, z;
	DWORD color;
};

class CCurveApplication : public CHostApplication  
{
public:
	
	HRESULT SetupDevice();
	CCurveApplication();
	virtual ~CCurveApplication();

	BOOL FillGridBuffer();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PostReset();
	virtual void Render();

	LPDIRECT3DVERTEXBUFFER8 m_pGridVertices;
};
