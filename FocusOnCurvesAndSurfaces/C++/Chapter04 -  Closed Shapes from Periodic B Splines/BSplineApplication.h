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
#define NUM_CONTROL_POINTS 6 
#define CURVE_ORDER        4

//define how many points are along the curve
#define NUM_CURVE_VERTICES   100

class CBSplineApplication : public CCurveApplication  
{
public:
	void DrawSlope();
	void DefineBasisFunctions();
	void SetKnotVector();
	CBSplineApplication();
	virtual ~CBSplineApplication();

	BOOL FillCurveBuffer();
	void DrawBasisFunctions();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual void Render();

	float        m_KnotVector[NUM_CONTROL_POINTS + CURVE_ORDER];

	//This 3D array is layed out as follows
	//[Values of t for each vertex][Resulting Basis function[i][k] for the value of t]

	float        m_BasisFunctions[NUM_CURVE_VERTICES][NUM_CONTROL_POINTS][CURVE_ORDER];
	float        m_DerivativeBasis[NUM_CURVE_VERTICES][NUM_CONTROL_POINTS][CURVE_ORDER];

	LPDIRECT3DVERTEXBUFFER8 m_pCurveVertices;

	float m_MinT;
	float m_MaxT;
};

#endif // !defined(AFX_POLYCURVEAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_)
