/***************************************************************
* CCurveApplication.cpp                                        *
*                                                              *
* This file contains the implementation of the                 *
* CCurveApplication class.    	            				   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

#include "CurveApplication.h"

//make sure the resolution divides evenly!!!
#define GRID_RESOLUTION   5
#define NUM_GRID_LINES    (2 + ((400 / GRID_RESOLUTION + 1) + (600 / GRID_RESOLUTION + 1)))
#define NUM_GRID_VERTICES (2 * NUM_GRID_LINES)


CCurveApplication::CCurveApplication()
{
	m_pGridVertices = NULL;
}

CCurveApplication::~CCurveApplication()
{
}

BOOL CCurveApplication::PostInitialize()
{	
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
						          D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return FALSE;

	//Set the basics
	SetupDevice();

	//Create the vertex buffer for the grid lines in video memory.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
						NUM_GRID_VERTICES * sizeof(GENERAL_VERTEX),
						D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
						D3DFVF_GENERALVERTEX,
						D3DPOOL_DEFAULT, &m_pGridVertices)))
		return FALSE;

	FillGridBuffer();
	
	return TRUE;
}

void CCurveApplication::Render()
{
	//These settings will never change in a curve application
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_PixelProjection);
	m_pD3DDevice->SetTransform(D3DTS_VIEW,  &m_ViewMatrix);
	m_pD3DDevice->SetTransform(D3DTS_WORLD,  &m_WorldMatrix);

	m_pD3DDevice->SetVertexShader(D3DFVF_GENERALVERTEX);

	m_pD3DDevice->SetStreamSource(0, m_pGridVertices, sizeof(GENERAL_VERTEX));

	m_pD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, NUM_GRID_LINES);
}

BOOL CCurveApplication::PostReset()
{
	SetupDevice();

	return TRUE;
}


HRESULT CCurveApplication::SetupDevice()
{
	//Create a pixel based ortho matrix
	RECT WindowRect;
	GetClientRect(m_hWnd, &WindowRect);

	//I'm setting things up so that the origin is slightly offset from the bottom
	//left corner. This will allow you to specify data points without accounting for any 
	//offsets, etc.
	D3DXMatrixOrthoLH(&m_PixelProjection, (float)WindowRect.right, (float)WindowRect.bottom, 0.0f, 1.0f);
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixTranslation(&m_WorldMatrix, -((float)WindowRect.right / 2.0f - 10.0f),
		                  -((float)WindowRect.bottom / 2.0f - 10.0f), 0.0f);

	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);	
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

//Release the vertex buffer before the device is destroyed
BOOL CCurveApplication::PreTerminate()
{
	if (m_pGridVertices)
	{
		m_pGridVertices->Release();
		m_pGridVertices = NULL;
	}
	return TRUE;
}

BOOL CCurveApplication::FillGridBuffer()
{
	//If we don't have a buffer, stop
	if (!m_pGridVertices)
		return FALSE;
	
	//Create a pointer to hold the locked vertices
	GENERAL_VERTEX *pVertices;

	//Lock the vertices.  If we fail, return false;
	if (FAILED(m_pGridVertices->Lock(0,
						NUM_GRID_VERTICES * sizeof(GENERAL_VERTEX),
						(BYTE **)&pVertices, D3DLOCK_DISCARD)))
		return FALSE;

	//clear everything
	memset(pVertices, 0, NUM_GRID_VERTICES * sizeof(GENERAL_VERTEX));

	//Build a line list for the grid. This is not the most efficient method,
	//but it is the clearest.
	int Index = 0;

	//horizontal lines
	for (int i = 0; i <= 400; i += GRID_RESOLUTION)
	{
		pVertices[Index].x = 0.0f;       pVertices[Index].y = (float)i;
		pVertices[Index + 1].x = 600.0f; pVertices[Index + 1].y = (float)i;

		pVertices[Index].z =     pVertices[Index + 1].z     = 1.0f;
		pVertices[Index].color = pVertices[Index + 1].color = 0xFFAAAAAA;

		Index += 2;
	}

	//vertical lines
	for (i = 0; i <= 600; i += GRID_RESOLUTION)
	{
		pVertices[Index].y = 0.0f;       pVertices[Index].x = (float)i;
		pVertices[Index + 1].y = 400.0f; pVertices[Index + 1].x = (float)i;

		pVertices[Index].z =     pVertices[Index + 1].z     = 1.0f;
		pVertices[Index].color = pVertices[Index + 1].color = 0xFFAAAAAA;

		Index += 2;
	}

	//axis lines
	pVertices[Index].x = 0.0f;       pVertices[Index].y = 0.0f;
	pVertices[Index + 1].x = 600.0f; pVertices[Index + 1].y = 0.0f;
	pVertices[Index + 2].x = 0.0f;   pVertices[Index + 2].y = 0.0f;
	pVertices[Index + 3].x = 0.0f;   pVertices[Index + 3].y = 400.0f;

	pVertices[Index].z = pVertices[Index + 1].z = pVertices[Index + 2].z = pVertices[Index + 3].z = 1.0f;
	pVertices[Index].color = pVertices[Index + 1].color = pVertices[Index + 2].color = pVertices[Index + 3].color = 0xFF000000;

	//Unlock the buffer and return.
	m_pGridVertices->Unlock();

	return TRUE;	
}


