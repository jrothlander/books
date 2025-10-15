// PolyPatchApplication.cpp: implementation of the CNURBSPatchApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "NURBSPatchApplication.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define NUM_PATCH_TRIANGLES (2 * (NUM_PATCH_VERTICES - 1)*(NUM_PATCH_VERTICES - 1))
#define NUM_TOTAL_VERTICES (NUM_U_POINTS * NUM_V_POINTS + NUM_PATCH_VERTICES * NUM_PATCH_VERTICES)

#define NUM_GRID_INDICES  ((2 * NUM_U_POINTS * (NUM_V_POINTS - 1)) + (2 * NUM_V_POINTS * (NUM_U_POINTS - 1)))
#define NUM_PATCH_INDICES (6 * NUM_PATCH_VERTICES * NUM_PATCH_VERTICES)

#define NUM_TOTAL_INDICES (NUM_GRID_INDICES + NUM_PATCH_INDICES)

#define ANIMATE(x) (x * sin((float)GetTickCount() / 1000.0f))

#define EPSILON 0.00001f


CNURBSPatchApplication::CNURBSPatchApplication()
{
}

CNURBSPatchApplication::~CNURBSPatchApplication()
{
}

BOOL CNURBSPatchApplication::PostInitialize()
{	
	CPatchApplication::PostInitialize();

	//Create the vertex buffer for the grid lines in video memory.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(NUM_TOTAL_VERTICES * sizeof(GENERAL_VERTEX),
						                        D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
						                        D3DFVF_GENERALVERTEX,
						                        D3DPOOL_DEFAULT, &m_pPatchVertices)))
		return FALSE;

	//Create the index buffer for the indexed triangle list of the mesh
	if (FAILED(m_pD3DDevice->CreateIndexBuffer(NUM_TOTAL_INDICES * sizeof(short),
						D3DUSAGE_WRITEONLY,
						D3DFMT_INDEX16,
						D3DPOOL_DEFAULT, &m_pPatchIndex)))
		return FALSE;

	//lock the index buffer once - these are hardcoded values
	short *pIndices;
	m_pPatchIndex->Lock(0, NUM_TOTAL_INDICES * sizeof(short), (BYTE**)&pIndices, 0);

	for (long UIndex = 0; UIndex < NUM_U_POINTS; UIndex++)
	{
		for (long VIndex = 0; VIndex < NUM_V_POINTS - 1; VIndex++)
		{
			*pIndices       = VIndex + (UIndex * NUM_V_POINTS);
			*(pIndices + 1) = VIndex + (UIndex * NUM_V_POINTS) + 1;

			pIndices = pIndices + 2;
		}
	}
	for (long VIndex = 0; VIndex < NUM_V_POINTS; VIndex++)
	{
		for (long UIndex = 0; UIndex < NUM_U_POINTS - 1; UIndex++)
		{
			*pIndices       = VIndex + (UIndex * NUM_V_POINTS);
			*(pIndices + 1) = VIndex + ((UIndex + 1) * NUM_V_POINTS);

			pIndices = pIndices + 2;
		}
	}

	//Fill the index for the mesh
	for (UIndex = 0; UIndex < NUM_PATCH_VERTICES - 1; UIndex++)
	{
		for (long VIndex = 0; VIndex < NUM_PATCH_VERTICES - 1; VIndex++)
		{
			*pIndices       = (NUM_U_POINTS * NUM_V_POINTS) + VIndex + (UIndex * NUM_PATCH_VERTICES);
			*(pIndices + 1) = (NUM_U_POINTS * NUM_V_POINTS) + VIndex + (UIndex * NUM_PATCH_VERTICES) + 1;
			*(pIndices + 2) = (NUM_U_POINTS * NUM_V_POINTS) + VIndex + ((UIndex + 1) * NUM_PATCH_VERTICES);
			*(pIndices + 3) = (NUM_U_POINTS * NUM_V_POINTS) + VIndex + (UIndex * NUM_PATCH_VERTICES) + 1;
			*(pIndices + 4) = (NUM_U_POINTS * NUM_V_POINTS) + VIndex + ((UIndex + 1) * NUM_PATCH_VERTICES) + 1;
			*(pIndices + 5) = (NUM_U_POINTS * NUM_V_POINTS) + VIndex + ((UIndex + 1) * NUM_PATCH_VERTICES);

			pIndices = pIndices + 6;
		}
	}

	//At this point, everything is set up!
	m_pPatchIndex->Unlock();

	SetKnotVectors();
	SetWeights();
	DefineBasisFunctions();

	return TRUE;
}

void CNURBSPatchApplication::SetKnotVectors()
{
	//This code creates an open uniform knot vector
	int KnotValue = 0;
	for (long i = 0; i < PATCH_ORDER_U + NUM_U_POINTS; i++)
	{
		if (i <= NUM_U_POINTS && i >= PATCH_ORDER_U)
			KnotValue++;

		m_KnotVectorU[i] = (float)KnotValue / (float)(NUM_U_POINTS - PATCH_ORDER_U + 1);
	}

	//This code creates an open uniform knot vector
	KnotValue = 0;
	for (i = 0; i < PATCH_ORDER_V + NUM_V_POINTS; i++)
	{
		if (i <= NUM_V_POINTS && i >= PATCH_ORDER_V)
			KnotValue++;

		m_KnotVectorV[i] = (float)KnotValue / (float)(NUM_V_POINTS - PATCH_ORDER_V + 1);
	}
}

void CNURBSPatchApplication::Render()
{
	CPatchApplication::Render();

	FillPatchBuffer();
	
	m_pD3DDevice->SetStreamSource(0, m_pPatchVertices, sizeof(GENERAL_VERTEX));

	//Draw the Patch
	//m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pD3DDevice->SetIndices(m_pPatchIndex, 0);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
		                               NUM_PATCH_VERTICES * NUM_PATCH_VERTICES,
									   NUM_GRID_INDICES, NUM_PATCH_TRIANGLES);

	//Draw the control vertices last so that you can see the control grid	
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, NUM_TOTAL_VERTICES, 0, (NUM_GRID_INDICES / 2));

	//DrawBasisFunctions();
}

//Release the vertex buffer before the device is destroyed
BOOL CNURBSPatchApplication::PreTerminate()
{
	CPatchApplication::PreTerminate();
	
	if (m_pPatchVertices)
	{
		m_pPatchVertices->Release();
		m_pPatchVertices = NULL;
	}

	if (m_pPatchIndex)
	{
		m_pPatchIndex->Release();
		m_pPatchIndex = NULL;
	}

	return TRUE;
}

void CNURBSPatchApplication::DefineBasisFunctions()
{
	//I am defining the parameter values for each curve vertex here
	//so make everything slightly clearer. You could define these values
	//somewhere else.

	//Find the Derivatives...
	//First, set all of the first order basis derivatives to zero - well, set all of them....
	memset(m_DerivativeBasisU, 0, NUM_PATCH_VERTICES * NUM_U_POINTS * PATCH_ORDER_U * sizeof(float));
	memset(m_DerivativeBasisV, 0, NUM_PATCH_VERTICES * NUM_V_POINTS * PATCH_ORDER_V * sizeof(float));

	//Fill the U values
	for (long Vertex = 0; Vertex < NUM_PATCH_VERTICES; Vertex++)
	{
		float u = (float)Vertex / (float)(NUM_PATCH_VERTICES - 1);

		//force the last compare to work
		if (u == 1.0f) u = 1.0f - EPSILON;

		//Find the first order basis functions
		for (long ControlPoint = 0; ControlPoint < NUM_U_POINTS; ControlPoint++)
		{
			if (u >= m_KnotVectorU[ControlPoint] && u < m_KnotVectorU[ControlPoint + 1])
				m_BasisFunctionsU[Vertex][ControlPoint][0] = 1.0f;
			else
				m_BasisFunctionsU[Vertex][ControlPoint][0] = 0.0f;
		}
	}

	//Fill the V values
	for (Vertex = 0; Vertex < NUM_PATCH_VERTICES; Vertex++)
	{
		float v = (float)Vertex / (float)(NUM_PATCH_VERTICES - 1);

		//force the last compare to work
		if (v == 1.0f) v = 1.0f - EPSILON;

		//Find the first order basis functions
		for (long ControlPoint = 0; ControlPoint < NUM_V_POINTS; ControlPoint++)
		{
			if (v >= m_KnotVectorV[ControlPoint] && v < m_KnotVectorV[ControlPoint + 1])
				m_BasisFunctionsV[Vertex][ControlPoint][0] = 1.0f;
			else
				m_BasisFunctionsV[Vertex][ControlPoint][0] = 0.0f;
		}
	}

	//Find the rest - loop through by order
	for (long Order = 1; Order < PATCH_ORDER_U; Order++)
	{
		//Then loop by control point...
		for (long ControlPoint = 0; ControlPoint < NUM_U_POINTS; ControlPoint++)
		{
			//Then loop by parameter value...
			for (long Vertex = 0; Vertex < NUM_PATCH_VERTICES; Vertex++)
			{
				//Obviously, make sure this is the same as above...
				float u = (float)Vertex / (float)(NUM_PATCH_VERTICES - 1);

				float Nikm1   = m_BasisFunctionsU[Vertex][ControlPoint][Order - 1];
				float Nip1km1 = m_BasisFunctionsU[Vertex][ControlPoint + 1][Order - 1];
				float Dikm1   = m_DerivativeBasisU[Vertex][ControlPoint][Order - 1];
				float Dip1km1 = m_DerivativeBasisU[Vertex][ControlPoint + 1][Order - 1];

				float xi    = m_KnotVectorU[ControlPoint];
				float xikm1 = m_KnotVectorU[ControlPoint + Order - 1 + 1];
				float xik   = m_KnotVectorU[ControlPoint + Order + 1];
				float xip1  = m_KnotVectorU[ControlPoint + 1];

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
					FirstTermBasis      = ((u - xi) * Nikm1) / (xikm1 - xi);
					FirstTermDerivative = (Nikm1 + ((u - xi) * Dikm1)) / (xikm1 - xi);
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
					SecondTermBasis = ((xik - u) * Nip1km1) / (xik - xip1);
					SecondTermDerivative = (((xik - u) * Dip1km1) - Nip1km1) / (xik - xip1);
				}

				m_BasisFunctionsU[Vertex][ControlPoint][Order]  = FirstTermBasis + SecondTermBasis;
				m_DerivativeBasisU[Vertex][ControlPoint][Order] = FirstTermDerivative + SecondTermDerivative;
			}
		}
	}

	//Find the rest - loop through by order
	for (Order = 1; Order < PATCH_ORDER_V; Order++)
	{
		//Then loop by control point...
		for (long ControlPoint = 0; ControlPoint < NUM_V_POINTS; ControlPoint++)
		{
			//Then loop by parameter value...
			for (long Vertex = 0; Vertex < NUM_PATCH_VERTICES; Vertex++)
			{
				//Obviously, make sure this is the same as above...
				float v = (float)Vertex / (float)(NUM_PATCH_VERTICES - 1);

				float Nikm1   = m_BasisFunctionsV[Vertex][ControlPoint][Order - 1];
				float Nip1km1 = m_BasisFunctionsV[Vertex][ControlPoint + 1][Order - 1];
				float Dikm1   = m_DerivativeBasisV[Vertex][ControlPoint][Order - 1];
				float Dip1km1 = m_DerivativeBasisV[Vertex][ControlPoint + 1][Order - 1];

				float xi    = m_KnotVectorV[ControlPoint];
				float xikm1 = m_KnotVectorV[ControlPoint + Order - 1 + 1];
				float xik   = m_KnotVectorV[ControlPoint + Order + 1];
				float xip1  = m_KnotVectorV[ControlPoint + 1];

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
					FirstTermBasis      = ((v - xi) * Nikm1) / (xikm1 - xi);
					FirstTermDerivative = (Nikm1 + ((v - xi) * Dikm1)) / (xikm1 - xi);
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
					SecondTermBasis = ((xik - v) * Nip1km1) / (xik - xip1);
					SecondTermDerivative = (((xik - v) * Dip1km1) - Nip1km1) / (xik - xip1);
				}

				m_BasisFunctionsV[Vertex][ControlPoint][Order]  = FirstTermBasis + SecondTermBasis;
				m_DerivativeBasisV[Vertex][ControlPoint][Order] = FirstTermDerivative + SecondTermDerivative;
			}
		}
	}
}


BOOL CNURBSPatchApplication::FillPatchBuffer()
{
	//If we don't have a buffer, stop
	if (!m_pPatchVertices)
		return FALSE;
	
	//Create a pointer to hold the locked vertices
	GENERAL_VERTEX *pVertices;

	//Lock the vertices.  If we fail, return false;
	//Create 4 more vertices for the control points
	if (FAILED(m_pPatchVertices->Lock(0, NUM_TOTAL_VERTICES * sizeof(GENERAL_VERTEX),
						              (BYTE **)&pVertices, D3DLOCK_DISCARD)))
		return FALSE;

	//Animate from a simple plane to a stylish plane
	float AnimFactor = 0.5f + (0.5f * sinf((float)GetTickCount() / 1000.0f));

	for (long U = 0; U < NUM_U_POINTS; U++)
	{
		for (long V = 0; V < NUM_V_POINTS; V++)
		{
			if (U == 0)
				pVertices[U * NUM_V_POINTS + V].x = 50.0f;
			else
				pVertices[U * NUM_V_POINTS + V].x = 200.0f + U * (250.0f / (float)(NUM_U_POINTS - 1));

			if (U == 0 || V == 0 || U == (NUM_U_POINTS - 1) || V == (NUM_V_POINTS - 1))
				pVertices[U * NUM_V_POINTS + V].y = 100.0f;
			else
				pVertices[U * NUM_V_POINTS + V].y = AnimFactor * 100.0f + (1.0f - AnimFactor) * 200.0f;

			if (V == 0)
				pVertices[U * NUM_V_POINTS + V].z = 50.0f;
			else if (V == (NUM_V_POINTS - 1))
				pVertices[U * NUM_V_POINTS + V].z = 450.0f;
			else
				pVertices[U * NUM_V_POINTS + V].z = 250.0f - 25.0f * (float)((NUM_V_POINTS - 1) / 2 - V);

			pVertices[U * NUM_V_POINTS + V].color = 0xFF0000FF;
		}
	}


	//Round the front
	pVertices[0].x = AnimFactor * pVertices[0].x  + (1.0f - AnimFactor) * 175.0f;
	pVertices[0].z = AnimFactor * pVertices[0].z  + (1.0f - AnimFactor) * 100.0f;
	pVertices[1].x = AnimFactor * pVertices[1].x  + (1.0f - AnimFactor) * 175.0f;
	pVertices[1].z = AnimFactor * pVertices[1].z  + (1.0f - AnimFactor) * 125.0f;

	pVertices[3].x = AnimFactor * pVertices[3].x  + (1.0f - AnimFactor) * -100.0f;
	pVertices[4].x = AnimFactor * pVertices[4].x  + (1.0f - AnimFactor) * -100.0f;
	pVertices[5].x = AnimFactor * pVertices[5].x  + (1.0f - AnimFactor) * -100.0f;

	pVertices[7].x = AnimFactor * pVertices[7].x  + (1.0f - AnimFactor) * 190.0f;
	pVertices[7].z = AnimFactor * pVertices[7].z  + (1.0f - AnimFactor) * 375.0f;
	pVertices[8].x = AnimFactor * pVertices[8].x  + (1.0f - AnimFactor) * 190.0f;
	pVertices[8].z = AnimFactor * pVertices[8].z  + (1.0f - AnimFactor) * 400.0f;

	//build the cockpit
	pVertices[12].x = AnimFactor * pVertices[12].x  + (1.0f - AnimFactor) * 50.0f;
	pVertices[13].x = AnimFactor * pVertices[13].x  + (1.0f - AnimFactor) * 50.0f;
	pVertices[14].x = AnimFactor * pVertices[14].x  + (1.0f - AnimFactor) * 50.0f;
	pVertices[21].y = AnimFactor * pVertices[21].y  + (1.0f - AnimFactor) * 250.0f;
	pVertices[22].y = AnimFactor * pVertices[22].y  + (1.0f - AnimFactor) * 250.0f;
	pVertices[23].y = AnimFactor * pVertices[23].y  + (1.0f - AnimFactor) * 250.0f;
	pVertices[31].y = AnimFactor * pVertices[31].y  + (1.0f - AnimFactor) * 225.0f;

	//pull out the tail
	pVertices[49].x = AnimFactor * pVertices[49].x  + (1.0f - AnimFactor) * 425.0f;
	//pVertices[49].y = AnimFactor * pVertices[49].y  + (1.0f - AnimFactor) * 175.0f;
	pVertices[57].x = AnimFactor * pVertices[57].x  + (1.0f - AnimFactor) * 525.0f;
	pVertices[58].x = AnimFactor * pVertices[58].x  + (1.0f - AnimFactor) * 525.0f;
	pVertices[59].x = AnimFactor * pVertices[59].x  + (1.0f - AnimFactor) * 525.0f;
	pVertices[66].x = AnimFactor * pVertices[66].x  + (1.0f - AnimFactor) * 575.0f;
	pVertices[67].x = AnimFactor * pVertices[67].x  + (1.0f - AnimFactor) * 625.0f;
	pVertices[67].y = AnimFactor * pVertices[67].y  + (1.0f - AnimFactor) * 125.0f;
	pVertices[68].x = AnimFactor * pVertices[68].x  + (1.0f - AnimFactor) * 575.0f;

	//Pull the body down
	pVertices[16].y = AnimFactor * pVertices[16].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[25].y = AnimFactor * pVertices[25].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[34].y = AnimFactor * pVertices[34].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[43].y = AnimFactor * pVertices[43].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[52].y = AnimFactor * pVertices[52].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[61].y = AnimFactor * pVertices[61].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[70].y = AnimFactor * pVertices[70].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[10].y = AnimFactor * pVertices[10].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[19].y = AnimFactor * pVertices[19].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[28].y = AnimFactor * pVertices[28].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[37].y = AnimFactor * pVertices[37].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[46].y = AnimFactor * pVertices[46].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[55].y = AnimFactor * pVertices[55].y  + (1.0f - AnimFactor) * 150.0f;
	pVertices[64].y = AnimFactor * pVertices[64].y  + (1.0f - AnimFactor) * 150.0f;

	//stretch the wings
	pVertices[35].z = AnimFactor * pVertices[35].z  + (1.0f - AnimFactor) * 510.0f;
	pVertices[44].z = AnimFactor * pVertices[44].z  + (1.0f - AnimFactor) * 550.0f;
	pVertices[53].z = AnimFactor * pVertices[53].z  + (1.0f - AnimFactor) * 610.0f;
	pVertices[62].z = AnimFactor * pVertices[62].z  + (1.0f - AnimFactor) * 630.0f;
	pVertices[71].z = AnimFactor * pVertices[71].z  + (1.0f - AnimFactor) * 650.0f;
	pVertices[80].z = AnimFactor * pVertices[80].z  + (1.0f - AnimFactor) * 450.0f;

	pVertices[63].z = AnimFactor * pVertices[63].z  + (1.0f - AnimFactor) *  150.0f;
	pVertices[63].z = AnimFactor * pVertices[63].z  + (1.0f - AnimFactor) * -150.0f;
	pVertices[54].z = AnimFactor * pVertices[54].z  + (1.0f - AnimFactor) * -130.0f;
	pVertices[45].z = AnimFactor * pVertices[45].z  + (1.0f - AnimFactor) * -110.0f;
	pVertices[36].z = AnimFactor * pVertices[36].z  + (1.0f - AnimFactor) * -50.0f;
	pVertices[27].z = AnimFactor * pVertices[27].z  + (1.0f - AnimFactor) * -10.0f;

	//Used to compute lighting
	D3DXVECTOR3 dPdU;
	D3DXVECTOR3 dPdV;
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 LightDirection(0.0f, 1.0f, 0.0f);

	//Set the Patch vertices
	for (U = 0; U < NUM_PATCH_VERTICES; U++)
	{
		for (long V = 0; V < NUM_PATCH_VERTICES; V++)
		{
			long Current = (NUM_U_POINTS * NUM_V_POINTS) + (U * NUM_PATCH_VERTICES) + V;

			pVertices[Current].x = 0.0f;
			pVertices[Current].y = 0.0f;
			pVertices[Current].z = 0.0f;
			memset(&dPdU, 0, sizeof(D3DXVECTOR3));
			memset(&dPdV, 0, sizeof(D3DXVECTOR3));

			float CurrentU = (float)U / (float)(NUM_PATCH_VERTICES - 1);
			float CurrentV = (float)V / (float)(NUM_PATCH_VERTICES - 1);

			float NumeratorMultiplier = 0.0f;
			float Denominator         = 0.0f;
			float DenominatorDU       = 0.0f;
			float DenominatorDV       = 0.0f;

			//loop through once to set up the NURBS numerators and denominators
			for (long UStep = 0; UStep < NUM_U_POINTS; UStep++)
			{
				for (long VStep = 0; VStep < NUM_V_POINTS; VStep++)
				{
					if (m_KnotVectorU[UStep] <= CurrentU && CurrentU <= m_KnotVectorU[UStep + PATCH_ORDER_U] &&
						m_KnotVectorV[VStep] <= CurrentV && CurrentV <= m_KnotVectorV[VStep + PATCH_ORDER_V])
					{
						Denominator += m_Weights[UStep][VStep] * 
							           m_BasisFunctionsU[U][UStep][PATCH_ORDER_U - 1] * 
									   m_BasisFunctionsV[V][VStep][PATCH_ORDER_V - 1];

						DenominatorDU += m_Weights[UStep][VStep] * 
							             m_DerivativeBasisU[U][UStep][PATCH_ORDER_U - 1] * 
										 m_BasisFunctionsV[V][VStep][PATCH_ORDER_V - 1];

						DenominatorDV += m_Weights[UStep][VStep] * 
							             m_BasisFunctionsU[U][UStep][PATCH_ORDER_U - 1] * 
										 m_DerivativeBasisV[V][VStep][PATCH_ORDER_V - 1];

						//scaling for the position
						NumeratorMultiplier = m_Weights[UStep][VStep] * 
							                  m_BasisFunctionsU[U][UStep][PATCH_ORDER_U - 1] * 
									          m_BasisFunctionsV[V][VStep][PATCH_ORDER_V - 1];

						long ControlIndex = UStep * NUM_V_POINTS + VStep;

						pVertices[Current].x += NumeratorMultiplier * pVertices[ControlIndex].x;
						pVertices[Current].y += NumeratorMultiplier * pVertices[ControlIndex].y;
						pVertices[Current].z += NumeratorMultiplier * pVertices[ControlIndex].z;

						//scaling for dP/dU
						NumeratorMultiplier = m_Weights[UStep][VStep] * 
							                  m_DerivativeBasisU[U][UStep][PATCH_ORDER_U - 1] * 
									          m_BasisFunctionsV[V][VStep][PATCH_ORDER_V - 1];

						dPdU.x += NumeratorMultiplier * pVertices[ControlIndex].x;
						dPdU.y += NumeratorMultiplier * pVertices[ControlIndex].y;
						dPdU.z += NumeratorMultiplier * pVertices[ControlIndex].z;

						//scaling for dP/dV
						NumeratorMultiplier = m_Weights[UStep][VStep] * 
							                  m_BasisFunctionsU[U][UStep][PATCH_ORDER_U - 1] * 
								              m_DerivativeBasisV[V][VStep][PATCH_ORDER_V - 1];

						dPdV.x += NumeratorMultiplier * pVertices[ControlIndex].x;
						dPdV.y += NumeratorMultiplier * pVertices[ControlIndex].y;
						dPdV.z += NumeratorMultiplier * pVertices[ControlIndex].z;
					}
				}
			}

			dPdU.x = ((dPdU.x / Denominator) - (pVertices[Current].x * DenominatorDU / (Denominator * Denominator)));
			dPdV.x = ((dPdV.x / Denominator) - (pVertices[Current].x * DenominatorDV / (Denominator * Denominator)));

			dPdU.y = ((dPdU.y / Denominator) - (pVertices[Current].y * DenominatorDU / (Denominator * Denominator)));
			dPdV.y = ((dPdV.y / Denominator) - (pVertices[Current].y * DenominatorDV / (Denominator * Denominator)));

			dPdU.z = ((dPdU.z / Denominator) - (pVertices[Current].z * DenominatorDU / (Denominator * Denominator)));
			dPdV.z = ((dPdV.z / Denominator) - (pVertices[Current].z * DenominatorDV / (Denominator * Denominator)));

			pVertices[Current].x /= Denominator;
			pVertices[Current].y /= Denominator;
			pVertices[Current].z /= Denominator;
						
			//Correct it down a bit so that you can see the control grid
			pVertices[Current].y -= 10.0f;

			D3DXVec3Normalize(&dPdU, &dPdU);
			D3DXVec3Normalize(&dPdV, &dPdV);

			D3DXVec3Cross(&Normal, &dPdV, &dPdU);
			D3DXVec3Normalize(&Normal, &Normal);

			float Intensity = D3DXVec3Dot(&Normal, &LightDirection);

			if (Intensity < 0.0f) Intensity = 0.0f;

			pVertices[Current].color = D3DCOLOR_COLORVALUE(Intensity, Intensity, Intensity, 1.0f);
		}
	}

	//Unlock the buffer and return.
	m_pPatchVertices->Unlock();

	return TRUE;	
}

void CNURBSPatchApplication::DrawBasisFunctions()
{
	//I'm recreating a new buffer here just to keep things simple. 
	//Remember, this is not a very efficient way to do this!!!
	LPDIRECT3DVERTEXBUFFER8 pBasisVertices;

	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
						NUM_PATCH_VERTICES * sizeof(GENERAL_VERTEX),
						D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
						D3DFVF_GENERALVERTEX,
						D3DPOOL_DEFAULT, &pBasisVertices)))
		return;

	//Create a pointer to hold the locked vertices
	GENERAL_VERTEX *pVertices;

	//AGAIN... not a very efficient way to go. Ideally, I'd pack
	//all the vertices for all the curves into one vertex buffer,
	//but I wanted to make things clear.

	for (long ControlPoint = 0; ControlPoint < NUM_U_POINTS; ControlPoint++)
	{
		//Lock the vertices.  If we fail, return;
		if (FAILED(pBasisVertices->Lock(0,
							NUM_PATCH_VERTICES * sizeof(GENERAL_VERTEX),
							(BYTE **)&pVertices, D3DLOCK_DISCARD)))
			return;

		for (long x = 0; x < NUM_PATCH_VERTICES; x++)
		{
			pVertices[x].x = x / (float)(NUM_PATCH_VERTICES - 1) * 500.0f;
			pVertices[x].y = 300.0f + 200.0f * m_BasisFunctionsU[x][ControlPoint][PATCH_ORDER_U - 1];
			pVertices[x].z = 0.0f;
			pVertices[x].color = 0x00000000;
		}

		pBasisVertices->Unlock();

		m_pD3DDevice->SetStreamSource(0, pBasisVertices, sizeof(GENERAL_VERTEX));
		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, NUM_PATCH_VERTICES - 1);
	}

	for (ControlPoint = 0; ControlPoint < NUM_V_POINTS; ControlPoint++)
	{
		//Lock the vertices.  If we fail, return;
		if (FAILED(pBasisVertices->Lock(0,
							NUM_PATCH_VERTICES * sizeof(GENERAL_VERTEX),
							(BYTE **)&pVertices, D3DLOCK_DISCARD)))
			return;

		for (long z = 0; z < NUM_PATCH_VERTICES; z++)
		{
			pVertices[z].x = 0.0f;
			pVertices[z].y = 300.0f + 200.0f * m_BasisFunctionsV[z][ControlPoint][PATCH_ORDER_V - 1];
			pVertices[z].z = z / (float)(NUM_PATCH_VERTICES - 1) * 500.0f;
			pVertices[z].color = 0x00000000;
		}

		pBasisVertices->Unlock();

		m_pD3DDevice->SetStreamSource(0, pBasisVertices, sizeof(GENERAL_VERTEX));
		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, NUM_PATCH_VERTICES - 1);
	}

	m_pD3DDevice->SetStreamSource(0, NULL, 0);
	pBasisVertices->Release();
}

void CNURBSPatchApplication::SetWeights()
{
	for (long u = 0; u < NUM_U_POINTS; u++)
	{
		for (long v = 0; v < NUM_V_POINTS; v++)
		{
			m_Weights[u][v] = 1.0f;
		}
	}
}

