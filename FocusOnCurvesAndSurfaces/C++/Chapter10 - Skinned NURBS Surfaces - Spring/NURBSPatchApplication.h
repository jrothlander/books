// PolyPatchApplication.h: interface for the CPolyPatchApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYPatchAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_)
#define AFX_POLYPatchAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PatchApplication.h"

#define NUM_PATCH_VERTICES_U 10
#define NUM_PATCH_VERTICES_V 60

#define NUM_U_POINTS 9
#define NUM_V_POINTS 20

#define PATCH_ORDER_U  3
#define PATCH_ORDER_V  3

class CNURBSPatchApplication : public CPatchApplication  
{
public:
	void SetWeights();
	void DefineBasisFunctions();
	void SetKnotVectors();

	CNURBSPatchApplication();
	virtual ~CNURBSPatchApplication();

	BOOL FillPatchBuffer();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual void Render();

	float        m_KnotVectorU[NUM_U_POINTS + PATCH_ORDER_U];
	float        m_KnotVectorV[NUM_V_POINTS + PATCH_ORDER_V];

	//This 3D array is layed out as follows
	//[Values of t for each vertex][Resulting Basis function[i][k] for the value of t]
	float        m_BasisFunctionsU[NUM_PATCH_VERTICES_U][NUM_U_POINTS][PATCH_ORDER_U];
	float        m_DerivativeBasisU[NUM_PATCH_VERTICES_U][NUM_U_POINTS][PATCH_ORDER_U];

	float        m_BasisFunctionsV[NUM_PATCH_VERTICES_V][NUM_V_POINTS][PATCH_ORDER_V];
	float        m_DerivativeBasisV[NUM_PATCH_VERTICES_V][NUM_V_POINTS][PATCH_ORDER_V];

	float        m_WeightsU[NUM_U_POINTS];
	float        m_WeightsV[NUM_V_POINTS];


	LPDIRECT3DVERTEXBUFFER8 m_pPatchVertices;
	LPDIRECT3DINDEXBUFFER8  m_pPatchIndex;
};

#endif // !defined(AFX_POLYPatchAPPLICATION_H__C7493EF7_0AD0_437F_BF31_2C2B1615F315__INCLUDED_)
