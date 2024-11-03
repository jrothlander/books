/***************************************************************
* TechniqueApplication.h                                       *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* serve as a base for all the techniques.  For this file to    *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

class CTechniqueApplication : public CHostApplication  
{
public:
	HRESULT CreateShaders();
	HRESULT ExtractBuffers();
	void SetupDevice();
	HRESULT LoadMesh();
	CTechniqueApplication();
	virtual ~CTechniqueApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();
	virtual BOOL HandleMessage(MSG *pMessage);

	LPD3DXMESH              m_pMesh;
	LPDIRECT3DVERTEXBUFFER8 m_pMeshVertexBuffer;
	LPDIRECT3DINDEXBUFFER8  m_pMeshIndexBuffer;

	D3DMATERIAL8 *m_pMeshMaterials;
	DWORD         m_NumMaterials;

	DWORD m_SineShaderHandle;
	DWORD m_ScaleShaderHandle;

	//If this value is 0, use the sine shader, otherwise,
	//use the scale shader.
	BOOL m_WhichShader;
};
