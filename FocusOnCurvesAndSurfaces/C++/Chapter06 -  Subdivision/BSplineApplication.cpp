// PolyCurveApplication.cpp: implementation of the CBSplineApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "BSplineApplication.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define ANIMATE(x) (x * sin((float)GetTickCount() / 1000.0f))

#define EPSILON 0.00001f

CBSplineApplication::CBSplineApplication()
{
	m_pHead = NULL;
	m_pCurveVertices = NULL;
}

CBSplineApplication::~CBSplineApplication()
{
}

BOOL CBSplineApplication::PostInitialize()
{	
	CCurveApplication::PostInitialize();

	SetKnotVector();
	CreateControlPoints();

	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
					(NUM_CONTROL_POINTS + MAX_CURVE_VERTICES) * sizeof(GENERAL_VERTEX),
					D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
					D3DFVF_GENERALVERTEX,
					D3DPOOL_DEFAULT, &m_pCurveVertices)))
	return FALSE;

	return TRUE;
}

void CBSplineApplication::Render()
{
	CCurveApplication::Render();

	SubdivideCurve(100, 0.0f);
	FillCurveBuffer(0xFFFF0000);
	m_pD3DDevice->SetStreamSource(0, m_pCurveVertices, sizeof(GENERAL_VERTEX));

	//Draw the control vertices last so that you can see the control polygon
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, NUM_CONTROL_POINTS - 1);

	//Draw the curve
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, NUM_CONTROL_POINTS, m_NumVertices - 1);

	//Draw the curve
	SubdivideCurve(30, 0.0f);
	FillCurveBuffer(0xFF0000FF);
	m_pD3DDevice->SetStreamSource(0, m_pCurveVertices, sizeof(GENERAL_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, NUM_CONTROL_POINTS, m_NumVertices - 1);

	m_pD3DDevice->SetStreamSource(0, NULL, 0);
}

//Release the vertex buffer before the device is destroyed
BOOL CBSplineApplication::PreTerminate()
{
	CCurveApplication::PreTerminate();
	
	if (m_pCurveVertices)
	{
		m_pCurveVertices->Release();
		m_pCurveVertices = NULL;
	}

	//Clean up the vertices
	SD_VERTEX *pTemp;
	while (m_pHead)
	{
		pTemp = m_pHead;
		m_pHead = m_pHead->pNextVertex;
		delete pTemp;
	}

	return TRUE;
}

BOOL CBSplineApplication::FillCurveBuffer(long Color)
{	
	//Create a pointer to hold the locked vertices
	GENERAL_VERTEX *pVertices;

	//Lock the vertices.  If we fail, return false;
	if (FAILED(m_pCurveVertices->Lock(0,
						(NUM_CONTROL_POINTS + m_NumVertices) * sizeof(GENERAL_VERTEX),
						(BYTE **)&pVertices, D3DLOCK_DISCARD)))
		return FALSE;

	for (int i = 0; i < NUM_CONTROL_POINTS; i++)
	{
		pVertices[i].x     = m_ControlPoints[i].x;
		pVertices[i].y     = m_ControlPoints[i].y;
		pVertices[i].z     = 1.0f;
		pVertices[i].color = 0xFF000000;
	}

	SD_VERTEX *pCurrent = m_pHead;

	while (pCurrent)
	{
		pVertices[i].x     = pCurrent->x;
		pVertices[i].y     = pCurrent->y;
		pVertices[i].z     = 1.0f;
		pVertices[i].color = Color;

		i++;
		pCurrent = pCurrent->pNextVertex;
	}


	//Unlock the buffer and return.
	m_pCurveVertices->Unlock();

	return TRUE;	
}

void CBSplineApplication::SetKnotVector()
{
	//This code creates an open uniform knot vector
	int KnotValue = 0;
	for (long i = 0; i < CURVE_ORDER + NUM_CONTROL_POINTS; i++)
	{
		if (i <= NUM_CONTROL_POINTS && i >= CURVE_ORDER)
			KnotValue++;

		m_KnotVector[i] = (float)KnotValue / (float)(NUM_CONTROL_POINTS - CURVE_ORDER + 1);
	}
}

void CBSplineApplication::GetPositionForT(float t, float *pX, float *pY)
{
	//force the endpoint to compare
	if (t == 1.0f) t = 1.0f - EPSILON;

	//Find the first order basis functions
	for (long ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
	{
		if ((t >= m_KnotVector[ControlPoint] && t < m_KnotVector[ControlPoint + 1]))
			m_BasisFunctions[ControlPoint][0] = 1.0f;
		else
			m_BasisFunctions[ControlPoint][0] = 0.0f;
	}

	//Find the rest - loop through by order
	for (long Order = 1; Order < CURVE_ORDER; Order++)
	{
		//Then loop by control point...
		for (long ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
		{
			float Nikm1   = m_BasisFunctions[ControlPoint][Order - 1];
			float Nip1km1 = m_BasisFunctions[ControlPoint + 1][Order - 1];

			float xi    = m_KnotVector[ControlPoint];
			float xikm1 = m_KnotVector[ControlPoint + Order];
			float xik   = m_KnotVector[ControlPoint + Order + 1];
			float xip1  = m_KnotVector[ControlPoint + 1];

			//Establish the 0/0 = 0 convention
			
			float FirstTermBasis;
			if (fabs(xikm1 - xi) < EPSILON)
				FirstTermBasis      = 0.0f;
			else
				FirstTermBasis      = ((t - xi) * Nikm1) / (xikm1 - xi);

			float SecondTermBasis;
			if (fabs(xik - xip1) < EPSILON)
				SecondTermBasis      = 0.0f;
			else
				SecondTermBasis = ((xik - t) * Nip1km1) / (xik - xip1);

			m_BasisFunctions[ControlPoint][Order]  = FirstTermBasis + SecondTermBasis;
		}
	}

	*pX = *pY = 0.0f;

	//Loop through the control points
	for (ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
	{
		*pX += m_ControlPoints[ControlPoint].x * m_BasisFunctions[ControlPoint][CURVE_ORDER - 1];
		*pY += m_ControlPoints[ControlPoint].y * m_BasisFunctions[ControlPoint][CURVE_ORDER - 1];
	}
}

void CBSplineApplication::CreateControlPoints()
{
	for (long ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
	{
		m_ControlPoints[ControlPoint].x = 600.0f / (float)(NUM_CONTROL_POINTS - 1) * (float)ControlPoint;
		m_ControlPoints[ControlPoint].y = 200.0f + (100.0f * sin(ControlPoint * 2.5f));

		//these members have no meaning in this context
		m_ControlPoints[ControlPoint].pNextVertex = NULL;
		m_ControlPoints[ControlPoint].t = 0.0f;
	}
}

void CBSplineApplication::SubdivideCurve(long NumPoints, float Tolerance)
{
	//Clean up the vertices
	SD_VERTEX *pTemp;
	while (m_pHead)
	{
		pTemp = m_pHead;
		m_pHead = m_pHead->pNextVertex;
		delete pTemp;
	}
	
	//First, create NUM_CONTROL_POINTS base vertices that we will subdivide from
	//NumPoints is assumed to be greater than NUM_CONTROL_POINTS.

	//Create the first point and build from there
	m_pHead = new SD_VERTEX;
	m_pHead->t = 0.0f;
	GetPositionForT(m_pHead->t, &(m_pHead->x), &(m_pHead->y));
	SD_VERTEX *pCurrent = m_pHead;

	for (int i = 1; i < NUM_CONTROL_POINTS; i++)
	{
		pCurrent->pNextVertex = new SD_VERTEX;
		pCurrent->pNextVertex->pNextVertex = NULL;

		pCurrent->pNextVertex->t = (float)i / (float)(NUM_CONTROL_POINTS - 1);

		GetPositionForT(pCurrent->pNextVertex->t, &(pCurrent->pNextVertex->x), &(pCurrent->pNextVertex->y));

		pCurrent = pCurrent->pNextVertex;
	}

	m_NumVertices = NUM_CONTROL_POINTS;

	while (TRUE)
	{
		if (NumPoints && m_NumVertices == NumPoints)
			break;

		SD_VERTEX *pBest = FindBestCandidate(Tolerance);

		if (pBest)
		{
			SD_VERTEX *pNew = new SD_VERTEX;
			pNew->t = pBest->t + (pBest->pNextVertex->t - pBest->t) / 2.0f;
			GetPositionForT(pNew->t, &(pNew->x), &(pNew->y));
			pNew->pNextVertex = pBest->pNextVertex;
			pBest->pNextVertex = pNew;

			m_NumVertices++;
		}
		else
			break;
	}
}

SD_VERTEX *CBSplineApplication::FindBestCandidate(float Tolerance)
{
	SD_VERTEX *pCurrent = m_pHead;

	SD_VERTEX *pBestCandidate = NULL;
	float      CurrentDistance = 0.0f;
	float tm;
	float xm, ym;
	float xi, yi;
	float Distance;

	while (pCurrent->pNextVertex)
	{
		tm = pCurrent->t + (pCurrent->pNextVertex->t - pCurrent->t) / 2.0f;

		GetPositionForT(tm, &xm, &ym);

		xi = pCurrent->x + 
                  (pCurrent->pNextVertex->x - pCurrent->x) / 2.0f;
		yi = pCurrent->y + 
                  (pCurrent->pNextVertex->y - pCurrent->y) / 2.0f;

		Distance = sqrt((xi - xm) * (xi - xm) + (yi - ym) * (yi - ym));

		if (Distance > CurrentDistance && Distance > Tolerance)
		{
			pBestCandidate  = pCurrent;
			CurrentDistance = Distance;
		}

		pCurrent = pCurrent->pNextVertex;
	}

	return pBestCandidate;
}
