// PolyCurveApplication.cpp: implementation of the CNURBSApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "NURBSApplication.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define ANIMATE(x) (x * sin((float)GetTickCount() / 1000.0f))

#define EPSILON 0.00001f
#define SLOPE_WIDTH 50.0f

CNURBSApplication::CNURBSApplication()
{

}

CNURBSApplication::~CNURBSApplication()
{

}

BOOL CNURBSApplication::PostInitialize()
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
	SetWeights();
	DefineBasisFunctions();

	return TRUE;
}

void CNURBSApplication::Render()
{
	CCurveApplication::Render();

	//redefine everything since the weights are animated.
	SetWeights();
	DefineBasisFunctions();
	FillCurveBuffer();
	
	m_pD3DDevice->SetStreamSource(0, m_pCurveVertices, sizeof(GENERAL_VERTEX));

	//Draw the control vertices last so that you can see the control polygon
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, NUM_CONTROL_POINTS - 1);

	//Draw the curve
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, NUM_CONTROL_POINTS, NUM_CURVE_VERTICES - 1);

	//Draw the slope
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, NUM_CONTROL_POINTS + NUM_CURVE_VERTICES, 1);

	//Draw the basic functions
	//DrawBasisFunctions();
}

//Release the vertex buffer before the device is destroyed
BOOL CNURBSApplication::PreTerminate()
{
	CCurveApplication::PreTerminate();
	
	if (m_pCurveVertices)
	{
		m_pCurveVertices->Release();
		m_pCurveVertices = NULL;
	}
	return TRUE;
}

BOOL CNURBSApplication::FillCurveBuffer()
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

	for (long ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
	{
		pVertices[ControlPoint].z = 1.0f;
		pVertices[ControlPoint].color = 0xFF000000;
	}

	pVertices[0].x = 300.0f; pVertices[0].y = 0.0f;
	pVertices[1].x = 500.0f; pVertices[1].y = 0.0f;
	pVertices[2].x = 400.0f; pVertices[2].y = 173.0f;
	pVertices[3].x = 300.0f; pVertices[3].y = 346.0f;
	pVertices[4].x = 200.0f; pVertices[4].y = 173.0f;
	pVertices[5].x = 100.0f; pVertices[5].y = 0.0f;
	pVertices[6].x = 300.0f; pVertices[6].y = 0.0f;



	//Set the actual vertices
	for (long i = NUM_CONTROL_POINTS; i < NUM_CURVE_VERTICES + NUM_CONTROL_POINTS; i++)
	{
		//Loop through the control points
		for (ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
		{
			pVertices[i].x += pVertices[ControlPoint].x * m_NURBSBasisFunctions[i - NUM_CONTROL_POINTS][ControlPoint][CURVE_ORDER - 1];
			pVertices[i].y += pVertices[ControlPoint].y * m_NURBSBasisFunctions[i - NUM_CONTROL_POINTS][ControlPoint][CURVE_ORDER - 1];
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
		PointX += pVertices[ControlPoint].x * m_NURBSBasisFunctions[Point][ControlPoint][CURVE_ORDER - 1];
		PointY += pVertices[ControlPoint].y * m_NURBSBasisFunctions[Point][ControlPoint][CURVE_ORDER - 1];
	}

	//Find the slope components using the derivatives
	float SlopeRun  = 0.0f;
	float SlopeRise = 0.0f; 
	
	//Loop through the control points
	for (ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
	{
		SlopeRun  += pVertices[ControlPoint].x * m_NURBSDerivativeBasis[Point][ControlPoint][CURVE_ORDER - 1];
		SlopeRise += pVertices[ControlPoint].y * m_NURBSDerivativeBasis[Point][ControlPoint][CURVE_ORDER - 1];
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

void CNURBSApplication::DrawBasisFunctions()
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
			pVertices[x].y = 200.0f * m_NURBSBasisFunctions[x][ControlPoint][CURVE_ORDER - 1];
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

void CNURBSApplication::SetKnotVector()
{
	m_KnotVector[0] = m_KnotVector[1] = m_KnotVector[2] = 0.0f;
	m_KnotVector[3] = m_KnotVector[4] = 0.333f;
	m_KnotVector[5] = m_KnotVector[6] = 0.666f;
	m_KnotVector[7] = m_KnotVector[8] = m_KnotVector[9] = 1.0f;
}

void CNURBSApplication::DefineBasisFunctions()
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

	//At this point, you have the evaluated spline basis functions. Now, apply the weight values to come up
	//with the evaluated NURBS basis functions.
	for (Order = 1; Order < CURVE_ORDER; Order++)
	{
		//Then loop by control point...
		for (long ControlPoint = 0; ControlPoint < NUM_CONTROL_POINTS; ControlPoint++)
		{
			//Then loop by parameter value...
			for (long Vertex = 0; Vertex < NUM_CURVE_VERTICES; Vertex++)
			{
				float Denominator = 0.0f;
				float SumDerivatives = 0.0f;

				//loop through the array of weights to sum the denominator and the numerator of the
				//second term of the derivative equation.
				for (long ControlWeight = 0; ControlWeight < NUM_CONTROL_POINTS; ControlWeight++)
				{
					Denominator += m_Weights[ControlWeight] * m_BasisFunctions[Vertex][ControlWeight][Order];
					SumDerivatives += m_Weights[ControlWeight] * m_DerivativeBasis[Vertex][ControlWeight][Order];
				}

				//Now, redefine the basis function
				m_NURBSBasisFunctions[Vertex][ControlPoint][Order] = m_Weights[ControlPoint] * 
					                                                 m_BasisFunctions[Vertex][ControlPoint][Order] /
																     Denominator;

				//Find the derivative. See the text for the actual equation.
				m_NURBSDerivativeBasis[Vertex][ControlPoint][Order] = (m_Weights[ControlPoint] * 
					                                                  m_DerivativeBasis[Vertex][ControlPoint][Order] /
																	  Denominator) - 
																	  (m_Weights[ControlPoint] * 
																	  m_BasisFunctions[Vertex][ControlPoint][Order] *
																	  SumDerivatives / (Denominator * Denominator));
			}
		}
	}
}


void CNURBSApplication::SetWeights()
{
	//set all the weights to 1.0
	for (long i = 0; i < NUM_CONTROL_POINTS; i++)
	{
		m_Weights[i] = 1.0f;
	}

	//reset whichever weights you want here
	m_Weights[1] = m_Weights[3] = m_Weights[5] = 0.5f;
}
