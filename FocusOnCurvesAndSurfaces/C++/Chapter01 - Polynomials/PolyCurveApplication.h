// PolyCurveApplication.h: interface for the CPolyCurveApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYCURVEAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_)
#define AFX_POLYCURVEAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CurveApplication.h"

class CPolyCurveApplication : public CCurveApplication  
{
public:
	CPolyCurveApplication();
	virtual ~CPolyCurveApplication();

	BOOL FillCurveBuffer();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual void Render();

	LPDIRECT3DVERTEXBUFFER8 m_pCurveVertices;
	LPDIRECT3DVERTEXBUFFER8 m_pSlopeVertices;
};

#endif // !defined(AFX_POLYCURVEAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_)
