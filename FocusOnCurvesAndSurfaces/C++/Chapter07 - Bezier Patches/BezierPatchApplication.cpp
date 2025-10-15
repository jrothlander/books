// PolyPatchApplication.cpp: implementation of the CBezierPatchApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "BezierPatchApplication.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//This is now the number of vertices in each direction
#define NUM_PATCH_VERTICES 20
#define NUM_PATCH_TRIANGLES (2 * (NUM_PATCH_VERTICES - 1)*(NUM_PATCH_VERTICES - 1))

#define NUM_U_POINTS 4
#define NUM_V_POINTS 4

#define NUM_TOTAL_VERTICES (NUM_U_POINTS * NUM_V_POINTS + NUM_PATCH_VERTICES * NUM_PATCH_VERTICES)

#define NUM_GRID_INDICES  ((2 * NUM_U_POINTS * (NUM_V_POINTS - 1)) + (2 * NUM_V_POINTS * (NUM_U_POINTS - 1)))
#define NUM_PATCH_INDICES (6 * NUM_PATCH_VERTICES * NUM_PATCH_VERTICES)

#define NUM_TOTAL_INDICES (NUM_GRID_INDICES + NUM_PATCH_INDICES)

//Basis functions
#define B0(t) ((1.0f - t) * (1.0f - t) * (1.0f - t))
#define B1(t) (3.0f * t * (1.0f - t) * (1.0f - t))
#define B2(t) (3.0f * t * t * (1.0f - t))
#define B3(t) (t * t * t)

//Derivatives of the basis functions
#define dB0(t) ((6.0f * t) - (3.0f * t * t) - 3.0f)
#define dB1(t) (3.0f - (12.0f * t) + (9.0f * t * t))
#define dB2(t) ((6.0f * t) - (9.0f * t * t))
#define dB3(t) (3.0f * t * t)

#define ANIMATE(x) (x * sin((float)GetTickCount() / 1000.0f))

CBezierPatchApplication::CBezierPatchApplication()
{
}

CBezierPatchApplication::~CBezierPatchApplication()
{
}

BOOL CBezierPatchApplication::PostInitialize()
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
			*(pIndices + 4) = (NUM_U_POINTS * NUM_V_POINTS) + VIndex + ((UIndex + 1) * NUM_PATCH_VERTICES);
			*(pIndices + 5) = (NUM_U_POINTS * NUM_V_POINTS) + VIndex + ((UIndex + 1) * NUM_PATCH_VERTICES) + 1;

			pIndices = pIndices + 6;
		}
	}

	//At this point, everything is set up!
	m_pPatchIndex->Unlock();

	return TRUE;
}

void CBezierPatchApplication::Render()
{
	CPatchApplication::Render();

	FillPatchBuffer();
	
	m_pD3DDevice->SetStreamSource(0, m_pPatchVertices, sizeof(GENERAL_VERTEX));

	//Draw the Patch
	//m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pD3DDevice->SetIndices(m_pPatchIndex, 0);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
		                               NUM_TOTAL_VERTICES,
									   NUM_GRID_INDICES, NUM_PATCH_TRIANGLES);

	//Draw the control vertices last so that you can see the control grid	
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, NUM_TOTAL_VERTICES, 0, (NUM_GRID_INDICES / 2));
}

//Release the vertex buffer before the device is destroyed
BOOL CBezierPatchApplication::PreTerminate()
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

BOOL CBezierPatchApplication::FillPatchBuffer()
{
	//If we don't have a buffer, stop
	if (!m_pPatchVertices)
		return FALSE;
	
	//Create a pointer to hold the locked vertices
	GENERAL_VERTEX *pVertices;

	//Lock the vertices.  If we fail, return false;
	if (FAILED(m_pPatchVertices->Lock(0, NUM_TOTAL_VERTICES * sizeof(GENERAL_VERTEX),
						              (BYTE **)&pVertices, D3DLOCK_DISCARD)))
		return FALSE;

	//This sets a uniform grid in the U and V direction. 
	//U is the X direction and V is the z direction
	for (long U = 0; U < NUM_U_POINTS; U++)
	{
		for (long V = 0; V < NUM_V_POINTS; V++)
		{
			pVertices[U * NUM_V_POINTS + V].x = U * (500.0f / (float)(NUM_U_POINTS - 1));
			pVertices[U * NUM_V_POINTS + V].y = 100.0f;
			pVertices[U * NUM_V_POINTS + V].z = V * (500.0f / (float)(NUM_V_POINTS - 1));
			pVertices[U * NUM_V_POINTS + V].color = 0xFF000000;
		}
	}

	pVertices[0].y = 100.0f + ANIMATE(100.0f);
	pVertices[1].y = 100.0f + ANIMATE(100.0f);
	pVertices[2].y = 100.0f + ANIMATE(100.0f);
	pVertices[3].y = 100.0f + ANIMATE(100.0f);
	pVertices[15].y = 200.0f + ANIMATE(200.0f);
	pVertices[12].y = 200.0f + ANIMATE(200.0f);

	long MidPoint = NUM_V_POINTS * NUM_U_POINTS / 2 + NUM_V_POINTS / 2;

	pVertices[MidPoint].y = 500.0f;
	pVertices[MidPoint].x = pVertices[MidPoint].x + ANIMATE(800.0f);
	pVertices[MidPoint - 1].y = 500.0f;
	pVertices[MidPoint - 1].x = pVertices[MidPoint - 1].x + ANIMATE(800.0f);

	//These values hold the computed values of the basis functions at each vertex
	float BU[4];
	float BV[4];

	//These values hold the computed values of the derivatives at each vertex
	float DU[4];
	float DV[4];

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
			float UVal = (float)U / (float)(NUM_PATCH_VERTICES - 1);
			float VVal = (float)V / (float)(NUM_PATCH_VERTICES - 1);
			BU[0] = B0(UVal); BU[1] = B1(UVal); BU[2] = B2(UVal); BU[3] = B3(UVal); 
			BV[0] = B0(VVal); BV[1] = B1(VVal); BV[2] = B2(VVal); BV[3] = B3(VVal); 
			DU[0] = dB0(UVal); DU[1] = dB1(UVal); DU[2] = dB2(UVal); DU[3] = dB3(UVal); 
			DV[0] = dB0(VVal); DV[1] = dB1(VVal); DV[2] = dB2(VVal); DV[3] = dB3(VVal); 

			long Current = (NUM_U_POINTS * NUM_V_POINTS) + (U * NUM_PATCH_VERTICES) + V;

			pVertices[Current].x = 0.0f;
			pVertices[Current].y = 0.0f;
			pVertices[Current].z = 0.0f;
			memset(&dPdU, 0, sizeof(D3DXVECTOR3));
			memset(&dPdV, 0, sizeof(D3DXVECTOR3));

			for (long UStep = 0; UStep < 4; UStep++)
			{
				for (long VStep = 0; VStep < 4; VStep++)
				{
					pVertices[Current].x += BU[UStep] * BV[VStep] * pVertices[UStep * 4 + VStep].x;
					pVertices[Current].y += BU[UStep] * BV[VStep] * pVertices[UStep * 4 + VStep].y;
					pVertices[Current].z += BU[UStep] * BV[VStep] * pVertices[UStep * 4 + VStep].z;

					dPdU.x += DU[UStep] * BV[VStep] * pVertices[UStep * 4 + VStep].x;
					dPdU.y += DU[UStep] * BV[VStep] * pVertices[UStep * 4 + VStep].y;
					dPdU.z += DU[UStep] * BV[VStep] * pVertices[UStep * 4 + VStep].z;

					dPdV.x += BU[UStep] * DV[VStep] * pVertices[UStep * 4 + VStep].x;
					dPdV.y += BU[UStep] * DV[VStep] * pVertices[UStep * 4 + VStep].y;
					dPdV.z += BU[UStep] * DV[VStep] * pVertices[UStep * 4 + VStep].z;
				}
			}

			//Correct it down a bit so that you can see the control grid
			pVertices[Current].y -= 10.0f;

			D3DXVec3Normalize(&dPdU, &dPdU);
			D3DXVec3Normalize(&dPdV, &dPdV);

			D3DXVec3Cross(&Normal, &dPdV, &dPdU);

			float Intensity = D3DXVec3Dot(&Normal, &LightDirection);

			if (Intensity < 0.0f) Intensity = 0.0f;

			pVertices[Current].color = D3DCOLOR_COLORVALUE(Intensity, Intensity, Intensity, 1.0f);
		}
	}

	//Unlock the buffer and return.
	m_pPatchVertices->Unlock();

	return TRUE;	
}