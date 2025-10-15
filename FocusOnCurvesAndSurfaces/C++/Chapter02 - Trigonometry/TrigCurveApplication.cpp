// PolyCurveApplication.cpp: implementation of the CTrigCurveApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "TrigCurveApplication.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define CURVE_RESOLUTION   1
#define NUM_CURVE_VERTICES (600 / CURVE_RESOLUTION + 1)

//for the hockey player
//#define FUNCTION_THETA(x) (x + 50.0f * sin(0.1f * (float)x))
//#define DERIVATIVE_THETA(x) (1 + 5.0f * cos(0.1f * (float)x))

//for the stock market
#define GENERAL_TREND(x)  (350.0f * sin(0.0035f * (float)x))
#define RANDOM            (float)rand() / (float)RAND_MAX
#define DAILY_ACTIVITY(x) (40.0f * RANDOM * sin(0.035f * (float)x + 10.0f * RANDOM))
#define FUNCTION_THETA(x) GENERAL_TREND(x) + DAILY_ACTIVITY(x)
//the derivative does not work in this case because of the random factor
#define DERIVATIVE_THETA(x) (0)

//Basic sine wave shifted up so that you can see the entire wave
//#define FUNCTION_THETA(x) (200.0f * sin(0.05f * (float)x) + 200.0f)
//#define DERIVATIVE_THETA(x) (10.0f * cos(0.05f * (float)x))

#define SLOPE_WIDTH 50.0f

CTrigCurveApplication::CTrigCurveApplication()
{

}

CTrigCurveApplication::~CTrigCurveApplication()
{

}

BOOL CTrigCurveApplication::PostInitialize()
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

void CTrigCurveApplication::Render()
{
	CCurveApplication::Render();
	
	m_pD3DDevice->SetStreamSource(0, m_pCurveVertices, sizeof(GENERAL_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, NUM_CURVE_VERTICES - 1);

	//This is not a demonstration of efficient rendering!!!
	GENERAL_VERTEX *pVertices;

	//Update the slope vertices
	m_pSlopeVertices->Lock(0, 2 * sizeof(GENERAL_VERTEX), (BYTE **)&pVertices, D3DLOCK_DISCARD);

	//Set the current position along the curve
	float CurrentX = (float)((GetTickCount() / 100) % 600);

	//Get the slope at the current X position
	float CurrentSlope = DERIVATIVE_THETA(CurrentX);

	//The slope is the tangent of the angle of the slope line. We can find the
	//angle with arctangent and use that angle value to find the X and Y values
	//for the endpoints of the slope line. This is explained more in Chapter 2
	float CurrentAngle = atan(CurrentSlope);

	pVertices[0].x = CurrentX + SLOPE_WIDTH * cos(CurrentAngle);
	pVertices[0].y = FUNCTION_THETA(CurrentX) + SLOPE_WIDTH * sin(CurrentAngle);
	pVertices[0].z     = 1.0f;
	pVertices[0].color = 0xFF0000FF;

	pVertices[1].x = CurrentX - SLOPE_WIDTH * cos(CurrentAngle);
	pVertices[1].y = FUNCTION_THETA(CurrentX) - SLOPE_WIDTH * sin(CurrentAngle);
	pVertices[1].z     = 1.0f;
	pVertices[1].color = 0xFF0000FF;

	//Unlock the buffer and render
	m_pSlopeVertices->Unlock();

	m_pD3DDevice->SetStreamSource(0, m_pSlopeVertices, sizeof(GENERAL_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, 1);
}

//Release the vertex buffer before the device is destroyed
BOOL CTrigCurveApplication::PreTerminate()
{
	CCurveApplication::PreTerminate();
	
	if (m_pCurveVertices)
	{
		m_pCurveVertices->Release();
		m_pCurveVertices = NULL;
	}
	return TRUE;
}

BOOL CTrigCurveApplication::FillCurveBuffer()
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
		pVertices[Index].y     = FUNCTION_THETA((float)x);
		pVertices[Index].z     = 1.0f;
		pVertices[Index].color = 0xFFFF0000;

		Index ++;
	}

	//Unlock the buffer and return.
	m_pCurveVertices->Unlock();

	return TRUE;	
}



