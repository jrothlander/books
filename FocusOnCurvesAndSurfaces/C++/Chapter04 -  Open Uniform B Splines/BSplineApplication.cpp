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
#define SLOPE_WIDTH 50.0f

CBSplineApplication::CBSplineApplication()
{

}

CBSplineApplication::~CBSplineApplication()
{

}

BOOL CBSplineApplication::PostInitialize()
{	
	CCurveApplication::PostInitialize();

	//Create the vertex buffer for the grid lines in video memory.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
						(NUM_CONTROL_POINTS + NUM_CURVE_VERTICES + 2) * sizeof(GENERAL_VERTEX),
						D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
						D3DFVF_GENERALVERTEX,
						D3DPOOL_DEFAULT, &m_pCurveVertices)))
		return FALSE;

	//Make SURE you set the knot vector
	//BEFORE you define the basis functions
	SetKnotVector();
	DefineBasisFunctions();

	return TRUE;
}

void CBSplineApplication::Render()
{
	CCurveApplication::Render();

	FillCurveBuffer();
	
	m_pD3DDevice->SetStreamSource(0, m_pCurveVertices, sizeof(GENERAL_VERTEX));

	//Draw the control vertices last so that you can see the control polygon
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, NUM_CONTROL_POINTS - 1);

	//Draw the curve
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, NUM_CONTROL_POINTS, NUM_CURVE_VERTICES - 1);

	//Draw the slope
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, NUM_CONTROL_POINTS + NUM_CURVE_VERTICES, 1);

	//Draw the basic functions
	DrawBasisFunctions();
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
	return TRUE;
}

BOOL CBSplineApplication::FillCurveBuffer()
{
	//If we don't have a buffer, stop
	if (!m_pCurveVertices)
		return FALSE;
	
	//Create a pointer to hold the locked vertices
	GENERAL_VERTEX *pVertices;

	//Lock the vertices.  If we fail, return false;
	//Create 4 more vertices for the control points
	if (FAILED(m_pCurveVertices->Lock(0,
						(NUM_CONTROL_POINTS + NUM_CURVE_VERTICES + 2) * sizeof(GENERAL_VERTEX),
						(BYTE **)&pVertices, D3DLOCK_DISCARD)))
		return FALSE;

	memset(pVertices, 0, (NUM_CONTROL_POINTS + NUM_CURVE_VERTICES + 2) * sizeof(GENERAL_VERTEX));

	//Set the control vertices
	for (long ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
	{
		pVertices[ControlPoint].x = (600.0f / (float)(NUM_CONTROL_POINTS - 1) * (float)ControlPoint);
		pVertices[ControlPoint].y = 200.0f + ANIMATE(200.0f * sin(pVertices[ControlPoint].x));
		pVertices[ControlPoint].z = 1.0f;
		pVertices[ControlPoint].color = 0xFF000000;
	}


	//Set the actual vertices
	for (long i = NUM_CONTROL_POINTS; i < NUM_CURVE_VERTICES + NUM_CONTROL_POINTS; i++)
	{
		//Loop through the control points
		for (ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
		{
			pVertices[i].x += pVertices[ControlPoint].x * m_BasisFunctions[i - NUM_CONTROL_POINTS][ControlPoint][CURVE_ORDER - 1];
			pVertices[i].y += pVertices[ControlPoint].y * m_BasisFunctions[i - NUM_CONTROL_POINTS][ControlPoint][CURVE_ORDER - 1];
			pVertices[i].z = 1.0f;
			pVertices[i].color = 0xFFFF0000;
		}
	}

	//Set the slope vertices.
	//"Point" is now an index into the predefined array of basis functions
	long Point = (GetTickCount() / 100) % NUM_CURVE_VERTICES;

	float PointX = 0.0f;
	float PointY = 0.0f;

	//Loop through the control points
	for (ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
	{
		PointX += pVertices[ControlPoint].x * m_BasisFunctions[Point][ControlPoint][CURVE_ORDER - 1];
		PointY += pVertices[ControlPoint].y * m_BasisFunctions[Point][ControlPoint][CURVE_ORDER - 1];
	}

	//Find the slope components using the derivatives
	float SlopeRun  = 0.0f;
	float SlopeRise = 0.0f; 
	
	//Loop through the control points
	for (ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
	{
		SlopeRun  += pVertices[ControlPoint].x * m_DerivativeBasis[Point][ControlPoint][CURVE_ORDER - 1];
		SlopeRise += pVertices[ControlPoint].y * m_DerivativeBasis[Point][ControlPoint][CURVE_ORDER - 1];
	}

	float Slope = SlopeRise / SlopeRun;
	float Angle = atan(Slope);

	pVertices[NUM_CURVE_VERTICES + NUM_CONTROL_POINTS].x = PointX + SLOPE_WIDTH * cos(Angle);
	pVertices[NUM_CURVE_VERTICES + NUM_CONTROL_POINTS].y = PointY + SLOPE_WIDTH * sin(Angle);
	pVertices[NUM_CURVE_VERTICES + NUM_CONTROL_POINTS + 1].x = PointX - SLOPE_WIDTH * cos(Angle);
	pVertices[NUM_CURVE_VERTICES + NUM_CONTROL_POINTS + 1].y = PointY - SLOPE_WIDTH * sin(Angle);
	pVertices[NUM_CURVE_VERTICES + NUM_CONTROL_POINTS].z = 
		                          pVertices[NUM_CURVE_VERTICES + NUM_CONTROL_POINTS + 1].z = 1.0f;
	pVertices[NUM_CURVE_VERTICES + NUM_CONTROL_POINTS].color = 
		                          pVertices[NUM_CURVE_VERTICES + NUM_CONTROL_POINTS + 1].color = 0xFF0000FF;

	//Unlock the buffer and return.
	m_pCurveVertices->Unlock();

	return TRUE;	
}

void CBSplineApplication::DrawBasisFunctions()
{
	//I'm recreating a new buffer here just to keep things simple. 
	//Remember, this is not a very efficient way to do this!!!
	LPDIRECT3DVERTEXBUFFER8 pBasisVertices;

	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
						NUM_CURVE_VERTICES * sizeof(GENERAL_VERTEX),
						D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
						D3DFVF_GENERALVERTEX,
						D3DPOOL_DEFAULT, &pBasisVertices)))
		return;

	//Create a pointer to hold the locked vertices
	GENERAL_VERTEX *pVertices;

	//AGAIN... not a very efficient way to go. Ideally, I'd pack
	//all the vertices for all the curves into one vertex buffer,
	//but I wanted to make things clear.

	for (long ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
	{
		//Lock the vertices.  If we fail, return;
		if (FAILED(pBasisVertices->Lock(0,
							NUM_CURVE_VERTICES * sizeof(GENERAL_VERTEX),
							(BYTE **)&pVertices, D3DLOCK_DISCARD)))
			return;

		for (long x = 0; x < NUM_CURVE_VERTICES; x++)
		{
			pVertices[x].x = x / (float)(NUM_CURVE_VERTICES - 1) * 600.0f;
			pVertices[x].y = 200.0f * m_BasisFunctions[x][ControlPoint][CURVE_ORDER - 1];
			pVertices[x].z = 1.0f;
			pVertices[x].color = 0x00000000;
		}

		pBasisVertices->Unlock();

		m_pD3DDevice->SetStreamSource(0, pBasisVertices, sizeof(GENERAL_VERTEX));
		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, NUM_CURVE_VERTICES - 1);
	}

	m_pD3DDevice->SetStreamSource(0, NULL, 0);
	pBasisVertices->Release();
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

void CBSplineApplication::DefineBasisFunctions()
{
	//I am defining the parameter values for each curve vertex here
	//so make everything slightly clearer. You could define these values
	//somewhere else.

	//Find the Derivatives...
	//First, set all of the first order basis derivatives to zero - well, set all of them....
	memset(m_DerivativeBasis, 0, NUM_CURVE_VERTICES * NUM_CONTROL_POINTS * CURVE_ORDER * sizeof(float));

	//I'm just creating an evenly spaced set of t values from 0 to 1
	//and then creating the evaluated basis function based on that t and
	//the knot vector
	for (long Vertex = 0; Vertex < NUM_CURVE_VERTICES; Vertex++)
	{
		float t = (float)Vertex / (float)(NUM_CURVE_VERTICES - 1);

		//force the last compare to work
		if (t == 1.0f) t = 1.0f - EPSILON;

		//Find the first order basis functions
		for (long ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
		{
			if (t >= m_KnotVector[ControlPoint] && t < m_KnotVector[ControlPoint + 1])
				m_BasisFunctions[Vertex][ControlPoint][0] = 1.0f;
			else
				m_BasisFunctions[Vertex][ControlPoint][0] = 0.0f;
		}
	}

	//Find the rest - loop through by order
	for (long Order = 1; Order < CURVE_ORDER; Order++)
	{
		//Then loop by control point...
		for (long ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
		{
			//Then loop by parameter value...
			for (long Vertex = 0; Vertex < NUM_CURVE_VERTICES; Vertex++)
			{
				//Obviously, make sure this is the same as above...
				float t = (float)Vertex / (float)(NUM_CURVE_VERTICES - 1);

				float Nikm1   = m_BasisFunctions[Vertex][ControlPoint][Order - 1];
				float Nip1km1 = m_BasisFunctions[Vertex][ControlPoint + 1][Order - 1];
				float Dikm1   = m_DerivativeBasis[Vertex][ControlPoint][Order - 1];
				float Dip1km1 = m_DerivativeBasis[Vertex][ControlPoint + 1][Order - 1];

				float xi    = m_KnotVector[ControlPoint];
				float xikm1 = m_KnotVector[ControlPoint + Order - 1 + 1];////////////////////////
				float xik   = m_KnotVector[ControlPoint + Order + 1];  //////////////////////////
				float xip1  = m_KnotVector[ControlPoint + 1];

				//Establish the 0/0 = 0 convention
				
				float FirstTermBasis;
				float FirstTermDerivative;
				if (fabs(xikm1 - xi) < EPSILON)
				{
					FirstTermBasis      = 0.0f;
					FirstTermDerivative = 0.0f;
				}
				else
				{
					FirstTermBasis      = ((t - xi) * Nikm1) / (xikm1 - xi);
					FirstTermDerivative = (Nikm1 + ((t - xi) * Dikm1)) / (xikm1 - xi);
				}

				float SecondTermBasis;
				float SecondTermDerivative;
				if (fabs(xik - xip1) < EPSILON)
				{
					SecondTermBasis      = 0.0f;
					SecondTermDerivative = 0.0f;
				}
				else
				{
					SecondTermBasis = ((xik - t) * Nip1km1) / (xik - xip1);
					SecondTermDerivative = (((xik - t) * Dip1km1) - Nip1km1) / (xik - xip1);
				}

				m_BasisFunctions[Vertex][ControlPoint][Order]  = FirstTermBasis + SecondTermBasis;
				m_DerivativeBasis[Vertex][ControlPoint][Order] = FirstTermDerivative + SecondTermDerivative;
			}
		}
	}
}

