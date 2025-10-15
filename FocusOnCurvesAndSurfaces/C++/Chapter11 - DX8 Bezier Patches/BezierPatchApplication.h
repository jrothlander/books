// PolyPatchApplication.h: interface for the CPolyPatchApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYPatchAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_)
#define AFX_POLYPatchAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PatchApplication.h"



class CBezierPatchApplication : public CPatchApplication  
{
public:
	CBezierPatchApplication();
	virtual ~CBezierPatchApplication();

	BOOL FillPatchBuffer();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual void Render();

	LPDIRECT3DVERTEXBUFFER8 m_pPatchVertices;
	D3DRECTPATCH_INFO       m_PatchInfo;
	DWORD                   m_ShaderHandle;
};

#endif // !defined(AFX_POLYPatchAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_)
