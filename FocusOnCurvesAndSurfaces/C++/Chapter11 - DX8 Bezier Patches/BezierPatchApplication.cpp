// PolyPatchApplication.cpp: implementation of the CBezierPatchApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "BezierPatchApplication.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define NUM_U_POINTS 4
#define NUM_V_POINTS 4

#define NUM_TOTAL_VERTICES (NUM_U_POINTS * NUM_V_POINTS)

#define ANIMATE(x) (x * sinf((float)GetTickCount() / 1000.0f))

CBezierPatchApplication::CBezierPatchApplication()
{
	m_ShaderHandle = 0;
}

CBezierPatchApplication::~CBezierPatchApplication()
{
}

BOOL CBezierPatchApplication::PostInitialize()
{	
	CPatchApplication::PostInitialize();

	//Create the vertex buffer for the grid lines in video memory.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(NUM_TOTAL_VERTICES * sizeof(PATCH_VERTEX),
						                        D3DUSAGE_RTPATCHES | D3DUSAGE_DYNAMIC,
						                        D3DFVF_PATCHVERTEX,
						                        D3DPOOL_DEFAULT, &m_pPatchVertices)))
		return FALSE;

    //Set up the patch info structure - it doesn't change
	m_PatchInfo.StartVertexOffsetWidth  = 0;
    m_PatchInfo.StartVertexOffsetHeight = 0;
    m_PatchInfo.Width  = NUM_U_POINTS;
    m_PatchInfo.Height = NUM_V_POINTS;
    m_PatchInfo.Stride = NUM_U_POINTS;
    m_PatchInfo.Basis  = D3DBASIS_BEZIER;
    m_PatchInfo.Order  = D3DORDER_CUBIC;

    //set up the light and material. They don't change and they are both white
	D3DLIGHT8 Light;
    D3DMATERIAL8 Material;

	memset(&Light, 0, sizeof(D3DLIGHT8));
	memset(&Material, 0, sizeof(D3DMATERIAL8));

    Light.Type      = D3DLIGHT_DIRECTIONAL;
    Light.Diffuse.r = Light.Diffuse.g = Light.Diffuse.b = 1.0f;
    Light.Direction = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
    m_pD3DDevice->SetLight(0, &Light );
    m_pD3DDevice->LightEnable(0, TRUE );

    Material.Diffuse = Light.Diffuse;
    m_pD3DDevice->SetMaterial(&Material);

	//Set up the vertex declaration. This tells the system to tessellate the normals as well as the vertex positions
	DWORD Declaration[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),  
		D3DVSD_STREAM_TESS(),
		D3DVSD_TESSNORMAL(D3DVSDE_POSITION, D3DVSDE_NORMAL),     
		D3DVSD_END()
	};

	//create a shader based on that declaration
	m_pD3DDevice->CreateVertexShader(Declaration, NULL, &m_ShaderHandle, 0);

	return TRUE;
}

void CBezierPatchApplication::Render()
{
	CPatchApplication::Render();

    m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pD3DDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

	FillPatchBuffer();

    float NumSegments[4];
    NumSegments[0] = NumSegments[1] = NumSegments[2] = NumSegments[3] = 8.0f;
	m_pD3DDevice->SetVertexShader(m_ShaderHandle);
	m_pD3DDevice->SetStreamSource(0, m_pPatchVertices, sizeof(PATCH_VERTEX));

	//Draw the Patch
	//m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    m_pD3DDevice->DrawRectPatch(0, NumSegments, &m_PatchInfo);

    m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
}

//Release the vertex buffer before the device is destroyed
BOOL CBezierPatchApplication::PreTerminate()
{
	CPatchApplication::PreTerminate();

	if (m_ShaderHandle)
		m_pD3DDevice->DeleteVertexShader(m_ShaderHandle);
	
	if (m_pPatchVertices)
	{
		m_pPatchVertices->Release();
		m_pPatchVertices = NULL;
	}

	return TRUE;
}

BOOL CBezierPatchApplication::FillPatchBuffer()
{
	//If we don't have a buffer, stop
	if (!m_pPatchVertices)
		return FALSE;
	
	//Create a pointer to hold the locked vertices
	PATCH_VERTEX *pVertices;

	//Lock the vertices.  If we fail, return false;
	if (FAILED(m_pPatchVertices->Lock(0, NUM_TOTAL_VERTICES * sizeof(PATCH_VERTEX),
						              (BYTE **)&pVertices, 0)))
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

	//Unlock the buffer and return.
	m_pPatchVertices->Unlock();

	return TRUE;	
}