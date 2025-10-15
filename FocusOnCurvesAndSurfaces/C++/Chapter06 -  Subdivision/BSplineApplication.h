// PolyCurveApplication.h: interface for the CPolyCurveApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYCURVEAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_)
#define AFX_POLYCURVEAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CurveApplication.h"

//Define the parameters that drive the size of the knot vector,
//basis functions, etc.
#define NUM_CONTROL_POINTS 10
#define CURVE_ORDER        3

//define how many points are along the curve
#define MAX_CURVE_VERTICES   100

typedef struct SD_VERTEX
{
	float t;
	float x, y;
	SD_VERTEX *pNextVertex;
} SD_VERTEX;

class CBSplineApplication : public CCurveApplication  
{
public:
	SD_VERTEX * FindBestCandidate(float Tolerance);
	void SubdivideCurve(long NumPoints, float Tolerance);
	void CreateControlPoints();
	void GetPositionForT(float t, float *pX, float *pY);
	void SetKnotVector();
	CBSplineApplication();
	virtual ~CBSplineApplication();

	BOOL FillCurveBuffer(long Color);

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual void Render();

	float        m_KnotVector[NUM_CONTROL_POINTS + CURVE_ORDER];

	float m_BasisFunctions[NUM_CONTROL_POINTS][CURVE_ORDER];

	LPDIRECT3DVERTEXBUFFER8 m_pCurveVertices;

	SD_VERTEX *m_pHead;
	SD_VERTEX m_ControlPoints[NUM_CONTROL_POINTS];

	long m_NumVertices;
};

#endif // !defined(AFX_POLYCURVEAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_)
