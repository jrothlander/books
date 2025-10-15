// PolyCurveApplication.cpp: implementation of the CPolyCurveApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "PolyCurveApplication.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define CURVE_RESOLUTION   1
#define NUM_CURVE_VERTICES (600 / CURVE_RESOLUTION + 1)

#define FUNCTION_X(x) (0.00001f * (float)x * (float)x * (float)x)
#define DERIVATIVE_X(x) (0.00003f * (float)x * (float)x)

#define SLOPE_WIDTH 50.0f

CPolyCurveApplication::CPolyCurveApplication()
{
	m_pCurveVertices = NULL;
}

CPolyCurveApplication::~CPolyCurveApplication()
{

}

BOOL CPolyCurveApplication::PostInitialize()
{	
	CCurveApplication::PostInitialize();

	//Create the vertex buffer for the grid lines in video memory.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
						NUM_CURVE_VERTICES * sizeof(GENERAL_VERTEX),
						D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
						D3DFVF_GENERALVERTEX,
						D3DPOOL_DEFAULT, &m_pCurveVertices)))
		return FALSE;

	//Create a buffer for the slope animation
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
						2 * sizeof(GENERAL_VERTEX),
						D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
						D3DFVF_GENERALVERTEX,
						D3DPOOL_DEFAULT, &m_pSlopeVertices)))
		return FALSE;

	FillCurveBuffer();
	
	return TRUE;
}

void CPolyCurveApplication::Render()
{
	CCurveApplication::Render();
	
	m_pD3DDevice->SetStreamSource(0, m_pCurveVertices, sizeof(GENERAL_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, NUM_CURVE_VERTICES - 1);

	//This is not a demonstration of efficient rendering!!!
	GENERAL_VERTEX *pVertices;

	//Update the slope vertices
	m_pSlopeVertices->Lock(0, 2 * sizeof(GENERAL_VERTEX), (BYTE **)&pVertices, D3DLOCK_DISCARD);

	//Set the current position along the curve
	float CurrentX = (float)((GetTickCount() / 10) % 600);

	//Get the slope at the current X position
	float CurrentSlope = DERIVATIVE_X(CurrentX);

	//The slope is the tangent of the angle of the slope line. We can find the
	//angle with arctangent and use that angle value to find the X and Y values
	//for the endpoints of the slope line. This is explained more in Chapter 2
	float CurrentAngle = atan(CurrentSlope);

	pVertices[0].x = CurrentX + SLOPE_WIDTH * cos(CurrentAngle);
	pVertices[0].y = FUNCTION_X(CurrentX) + SLOPE_WIDTH * sin(CurrentAngle);
	pVertices[0].z     = 1.0f;
	pVertices[0].color = 0xFF0000FF;

	pVertices[1].x = CurrentX - SLOPE_WIDTH * cos(CurrentAngle);
	pVertices[1].y = FUNCTION_X(CurrentX) - SLOPE_WIDTH * sin(CurrentAngle);
	pVertices[1].z     = 1.0f;
	pVertices[1].color = 0xFF0000FF;

	//Unlock the buffer and render
	m_pSlopeVertices->Unlock();

	m_pD3DDevice->SetStreamSource(0, m_pSlopeVertices, sizeof(GENERAL_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, 1);
}

//Release the vertex buffer before the device is destroyed
BOOL CPolyCurveApplication::PreTerminate()
{
	CCurveApplication::PreTerminate();
	
	if (m_pCurveVertices)
	{
		m_pCurveVertices->Release();
		m_pCurveVertices = NULL;
	}
	return TRUE;
}

BOOL CPolyCurveApplication::FillCurveBuffer()
{
	//If we don't have a buffer, stop
	if (!m_pCurveVertices)
		return FALSE;
	
	//Create a pointer to hold the locked vertices
	GENERAL_VERTEX *pVertices;

	//Lock the vertices.  If we fail, return false;
	if (FAILED(m_pCurveVertices->Lock(0,
						NUM_CURVE_VERTICES * sizeof(GENERAL_VERTEX),
						(BYTE **)&pVertices, D3DLOCK_DISCARD)))
		return FALSE;

	//clear everything
	memset(pVertices, 0, NUM_CURVE_VERTICES * sizeof(GENERAL_VERTEX));

	int Index = 0;
	for (int x = 0; x <= 600; x += CURVE_RESOLUTION)
	{
		pVertices[Index].x     = (float)x;       
		pVertices[Index].y     = FUNCTION_X((float)x);
		pVertices[Index].z     = 1.0f;
		pVertices[Index].color = 0xFFFF0000;

		Index ++;
	}

	//Unlock the buffer and return.
	m_pCurveVertices->Unlock();

	return TRUE;	
}



