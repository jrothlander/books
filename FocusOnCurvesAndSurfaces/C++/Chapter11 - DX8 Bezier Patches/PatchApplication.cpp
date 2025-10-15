/***************************************************************
* CPatchApplication.cpp                                        *
*                                                              *
* This file contains the implementation of the                 *
* CPatchApplication class.    	            				   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

#include "PatchApplication.h"

//make sure the resolution divides evenly!!!
#define GRID_RESOLUTION   20
#define NUM_GRID_LINES    (3 + (6 * (1 + (500 / GRID_RESOLUTION))))
#define NUM_GRID_VERTICES (2 * NUM_GRID_LINES)

#define HEIGHT_DELTA  20.0f
#define ORBIT_DELTA   0.1f


CPatchApplication::CPatchApplication()
{
	m_ViewerHeight = 500.0f;
	m_ViewerAngle = D3DX_PI / 8.0f;
}

CPatchApplication::~CPatchApplication()
{
}

BOOL CPatchApplication::HandleMessage(MSG *pMessage)
{
	if (pMessage->message == WM_KEYDOWN && pMessage->wParam == VK_UP)
		m_ViewerHeight += HEIGHT_DELTA;

	if (pMessage->message == WM_KEYDOWN && pMessage->wParam == VK_DOWN)
		m_ViewerHeight -= HEIGHT_DELTA;

	if (pMessage->message == WM_KEYDOWN && pMessage->wParam == VK_LEFT)
		m_ViewerAngle += ORBIT_DELTA;

	if (pMessage->message == WM_KEYDOWN && pMessage->wParam == VK_RIGHT)
		m_ViewerAngle -= ORBIT_DELTA;

	D3DXMatrixLookAtLH(&m_ViewMatrix,
		               &D3DXVECTOR3(250.0f + 1000.0f * cos(m_ViewerAngle),
		                            m_ViewerHeight,
						            250.0f + 1000.0f * sin(m_ViewerAngle)),
					   &D3DXVECTOR3(250.0f, 200.0f, 250.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	return CHostApplication::HandleMessage(pMessage);
}

BOOL CPatchApplication::PostInitialize()
{	
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_REF, 
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

void CPatchApplication::Render()
{
	//These settings will never change in a Patch application
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_Projection);
	m_pD3DDevice->SetTransform(D3DTS_VIEW,  &m_ViewMatrix);
	m_pD3DDevice->SetTransform(D3DTS_WORLD,  &m_WorldMatrix);

	m_pD3DDevice->SetVertexShader(D3DFVF_GENERALVERTEX);

	m_pD3DDevice->SetStreamSource(0, m_pGridVertices, sizeof(GENERAL_VERTEX));

	m_pD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, NUM_GRID_LINES);
}

BOOL CPatchApplication::PostReset()
{
	SetupDevice();

	return TRUE;
}


HRESULT CPatchApplication::SetupDevice()
{
	//Create a pixel based ortho matrix
	RECT WindowRect;
	GetClientRect(m_hWnd, &WindowRect);

	//Changing to a 3D view...
	D3DXMatrixPerspectiveFovLH(&m_Projection, D3DX_PI / 4.0f, 1.33f, 1.0f, 3000.0f);

	D3DXMatrixLookAtLH(&m_ViewMatrix, &D3DXVECTOR3(800.0f, m_ViewerHeight, 800.0f),
									  &D3DXVECTOR3(250.0f, 100.0f, 250.0f),
									  &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	D3DXMatrixIdentity(&m_WorldMatrix);

	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);	
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

//Release the vertex buffer before the device is destroyed
BOOL CPatchApplication::PreTerminate()
{
	if (m_pGridVertices)
	{
		m_pGridVertices->Release();
		m_pGridVertices = NULL;
	}
	return TRUE;
}

BOOL CPatchApplication::FillGridBuffer()
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

	//horizontal lines on the XY plane
	for (int i = 0; i <= 500; i += GRID_RESOLUTION)
	{
		pVertices[Index].x = 0.0f;       pVertices[Index].y = (float)i;
		pVertices[Index + 1].x = 500.0f; pVertices[Index + 1].y = (float)i;

		pVertices[Index].z =     pVertices[Index + 1].z     = 0.0f;
		pVertices[Index].color = pVertices[Index + 1].color = 0xFF8888AA;

		Index += 2;
	}

	//vertical lines on the XY plane
	for (i = 0; i <= 500; i += GRID_RESOLUTION)
	{
		pVertices[Index].y = 0.0f;       pVertices[Index].x = (float)i;
		pVertices[Index + 1].y = 500.0f; pVertices[Index + 1].x = (float)i;

		pVertices[Index].z =     pVertices[Index + 1].z     = 0.0f;
		pVertices[Index].color = pVertices[Index + 1].color = 0xFF8888AA;

		Index += 2;
	}

	//horizontal lines on the XZ plane
	for (i = 0; i <= 500; i += GRID_RESOLUTION)
	{
		pVertices[Index].x = 0.0f;       pVertices[Index].z = (float)i;
		pVertices[Index + 1].x = 500.0f; pVertices[Index + 1].z = (float)i;

		pVertices[Index].y =     pVertices[Index + 1].y     = 0.0f;
		pVertices[Index].color = pVertices[Index + 1].color = 0xFFAA8888;

		Index += 2;
	}

	//vertical lines on the XZ plane
	for (i = 0; i <= 500; i += GRID_RESOLUTION)
	{
		pVertices[Index].z = 0.0f;       pVertices[Index].x = (float)i;
		pVertices[Index + 1].z = 500.0f; pVertices[Index + 1].x = (float)i;

		pVertices[Index].y =     pVertices[Index + 1].y     = 0.0f;
		pVertices[Index].color = pVertices[Index + 1].color = 0xFFAA8888;

		Index += 2;
	}

	//horizontal lines on the ZY plane
	for (i = 0; i <= 500; i += GRID_RESOLUTION)
	{
		pVertices[Index].y = 0.0f;       pVertices[Index].z = (float)i;
		pVertices[Index + 1].y = 500.0f; pVertices[Index + 1].z = (float)i;

		pVertices[Index].x =     pVertices[Index + 1].x     = 0.0f;
		pVertices[Index].color = pVertices[Index + 1].color = 0xFF88AA88;

		Index += 2;
	}

	//vertical lines on the ZY plane
	for (i = 0; i <= 500; i += GRID_RESOLUTION)
	{
		pVertices[Index].z = 0.0f;       pVertices[Index].y = (float)i;
		pVertices[Index + 1].z = 500.0f; pVertices[Index + 1].y = (float)i;

		pVertices[Index].x =     pVertices[Index + 1].x     = 0.0f;
		pVertices[Index].color = pVertices[Index + 1].color = 0xFF88AA88;

		Index += 2;
	}

	//axis lines
	pVertices[Index].x = 0.0f;       pVertices[Index].y = 0.0f;       pVertices[Index].z = 0.0f;
	pVertices[Index + 1].x = 500.0f; pVertices[Index + 1].y = 0.0f;   pVertices[Index + 1].z = 0.0f;
	pVertices[Index + 2].x = 0.0f;   pVertices[Index + 2].y = 0.0f;   pVertices[Index + 2].z = 0.0f;
	pVertices[Index + 3].x = 0.0f;   pVertices[Index + 3].y = 500.0f; pVertices[Index + 3].z = 0.0f;
	pVertices[Index + 4].x = 0.0f;   pVertices[Index + 4].y = 0.0f;   pVertices[Index + 4].z = 0.0f;
	pVertices[Index + 5].x = 0.0f;   pVertices[Index + 5].y = 0.0f;   pVertices[Index + 5].z = 500.0f;

	pVertices[Index].color = pVertices[Index + 1].color = pVertices[Index + 2].color = 
		                     pVertices[Index + 3].color = pVertices[Index + 4].color = 
							 pVertices[Index + 5].color = 0xFF000000;

	//Unlock the buffer and return.
	m_pGridVertices->Unlock();

	return TRUE;	
}


