// PolyCurveApplication.cpp: implementation of the CBezierCurveApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "BezierCurveApplication.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define CURVE_RESOLUTION   1
#define NUM_CURVE_VERTICES (100 / CURVE_RESOLUTION + 1)

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

#define SLOPE_WIDTH 50.0f

#define ANIMATE(x) (x * sin((float)GetTickCount() / 1000.0f))

CBezierCurveApplication::CBezierCurveApplication()
{

}

CBezierCurveApplication::~CBezierCurveApplication()
{

}

BOOL CBezierCurveApplication::PostInitialize()
{	
	CCurveApplication::PostInitialize();

	//Create the vertex buffer for the grid lines in video memory.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
						(6 + NUM_CURVE_VERTICES) * sizeof(GENERAL_VERTEX),
						D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
						D3DFVF_GENERALVERTEX,
						D3DPOOL_DEFAULT, &m_pCurveVertices)))
		return FALSE;

	return TRUE;
}

void CBezierCurveApplication::Render()
{
	CCurveApplication::Render();

	FillCurveBuffer();
	
	m_pD3DDevice->SetStreamSource(0, m_pCurveVertices, sizeof(GENERAL_VERTEX));


	//Draw the curve
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 6, NUM_CURVE_VERTICES - 1);

	//Draw the control vertices last so that you can see the control polygon
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 3);

	//Draw the slope line
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 4, 1);

	//Draw the basic functions
	DrawBasisFunctions();
}

//Release the vertex buffer before the device is destroyed
BOOL CBezierCurveApplication::PreTerminate()
{
	CCurveApplication::PreTerminate();
	
	if (m_pCurveVertices)
	{
		m_pCurveVertices->Release();
		m_pCurveVertices = NULL;
	}
	return TRUE;
}

BOOL CBezierCurveApplication::FillCurveBuffer()
{
	//If we don't have a buffer, stop
	if (!m_pCurveVertices)
		return FALSE;
	
	//Create a pointer to hold the locked vertices
	GENERAL_VERTEX *pVertices;

	//Lock the vertices.  If we fail, return false;
	//Create 4 more vertices for the control points
	if (FAILED(m_pCurveVertices->Lock(0,
						(6 + NUM_CURVE_VERTICES) * sizeof(GENERAL_VERTEX),
						(BYTE **)&pVertices, D3DLOCK_DISCARD)))
		return FALSE;

	//Set the control vertices
	pVertices[0].x = 100.0f + ANIMATE(100.0f);
	pVertices[0].y = 100.0f + ANIMATE(100.0f);
	pVertices[1].x = 300.0f;
	pVertices[1].y = 200.0f + ANIMATE(200.0f);
	pVertices[2].x = 400.0f + ANIMATE(50.0f);
	pVertices[2].y = 200.0f;
	pVertices[3].x = 600.0f;
	pVertices[3].y = 100.0f + ANIMATE(-100.0f);

	pVertices[0].color = pVertices[1].color = 
		                 pVertices[2].color = 
						 pVertices[3].color = 0xFF000000;

	pVertices[0].z = pVertices[1].z = 
		             pVertices[2].z = 
					 pVertices[3].z = 1.0f;

	//Set the curve vertices
	int Index = 0;
	for (int i = 0; i <= 100; i += CURVE_RESOLUTION)
	{
		float t = (float)i / 100.0f;
		
		pVertices[Index + 6].x     = (B0(t) * pVertices[0].x) + (B1(t) * pVertices[1].x) + 
			                         (B2(t) * pVertices[2].x) + (B3(t) * pVertices[3].x);       
		pVertices[Index + 6].y     = (B0(t) * pVertices[0].y) + (B1(t) * pVertices[1].y) + 
			                         (B2(t) * pVertices[2].y) + (B3(t) * pVertices[3].y);  
		pVertices[Index + 6].z     = 1.0f;
		pVertices[Index + 6].color = 0xFFFF0000;

		Index ++;
	}

	//Set the slope vertices - find the slope at t
	float Point = (float)((GetTickCount() / 100) % 100) / 100.0f;

	float PointX = (B0(Point) * pVertices[0].x) + (B1(Point) * pVertices[1].x) + 
			       (B2(Point) * pVertices[2].x) + (B3(Point) * pVertices[3].x);
	float PointY = (B0(Point) * pVertices[0].y) + (B1(Point) * pVertices[1].y) + 
			       (B2(Point) * pVertices[2].y) + (B3(Point) * pVertices[3].y);

	float SlopeRun  = (dB0(Point) * pVertices[0].x) + (dB1(Point) * pVertices[1].x) + 
			          (dB2(Point) * pVertices[2].x) + (dB3(Point) * pVertices[3].x);
	float SlopeRise = (dB0(Point) * pVertices[0].y) + (dB1(Point) * pVertices[1].y) + 
			          (dB2(Point) * pVertices[2].y) + (dB3(Point) * pVertices[3].y);

	float Slope = SlopeRise / SlopeRun;
	float Angle = atan(Slope);

	pVertices[4].x = PointX + SLOPE_WIDTH * cos(Angle);
	pVertices[4].y = PointY + SLOPE_WIDTH * sin(Angle);
	pVertices[5].x = PointX - SLOPE_WIDTH * cos(Angle);
	pVertices[5].y = PointY - SLOPE_WIDTH * sin(Angle);
	pVertices[4].z = pVertices[5].z = 1.0f;
	pVertices[4].color = pVertices[5].color = 0xFF0000FF;
	

	//Unlock the buffer and return.
	m_pCurveVertices->Unlock();

	return TRUE;	
}

void CBezierCurveApplication::DrawBasisFunctions()
{
	//I'm recreating a new buffer here just to keep things simple. 
	//Remember, this is not a very efficient way to do this!!!

	LPDIRECT3DVERTEXBUFFER8 pBasisVertices;

	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
						400 * sizeof(GENERAL_VERTEX),
						D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
						D3DFVF_GENERALVERTEX,
						D3DPOOL_DEFAULT, &pBasisVertices)))
		return;

	//Create a pointer to hold the locked vertices
	GENERAL_VERTEX *pVertices;

	//Lock the vertices.  If we fail, return;
	if (FAILED(pBasisVertices->Lock(0,
						400 * sizeof(GENERAL_VERTEX),
						(BYTE **)&pVertices, D3DLOCK_DISCARD)))
		return;

	for (long x = 0; x < 100; x++)
	{
		float t = (float)x / 99.0f;
		
		pVertices[x].x           = x * 6;   
		pVertices[x + 100].x     = x * 6;   
		pVertices[x + 200].x     = x * 6;   
		pVertices[x + 300].x     = x * 6;   
		pVertices[x].y           = 400.0f * B0(t);
		pVertices[x + 100].y     = 400.0f * B1(t);
		pVertices[x + 200].y     = 400.0f * B2(t);
		pVertices[x + 300].y     = 400.0f * B3(t);
		pVertices[x].z = pVertices[x + 100].z = 
			             pVertices[x + 200].z = 
						 pVertices[x + 300].z = 1.0f;
		pVertices[x].color = pVertices[x + 100].color = 
			                 pVertices[x + 200].color = 
							 pVertices[x + 300].color = 0xFFFF0000;
	}

	pBasisVertices->Unlock();

	m_pD3DDevice->SetStreamSource(0, pBasisVertices, sizeof(GENERAL_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 99);
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 100, 99);
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 200, 99);
	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 300, 99);

	pBasisVertices->Release();
}



