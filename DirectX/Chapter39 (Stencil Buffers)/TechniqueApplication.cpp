/***************************************************************
* TechniqueApplication.cpp                                     *
*                                                              *
* This file contains the implementation of the                 *
* TechniqueApplication class.    	        				   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

#include "TechniqueApplication.h"

#define D3DFVF_MESHVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)
#define D3DFVF_STENCILVERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1)

struct MESH_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
	DWORD color;
};

struct STENCIL_VERTEX
{
	float x, y, z, rhw;
	float u, v;
};

#define CIRCLE_RES 15
#define NUM_VERTICES 29

CTechniqueApplication::CTechniqueApplication()
{
	m_pStencilVertexBuffer     = NULL;
	m_pMeshVertexBuffer        = NULL;
	m_pMeshIndexBuffer         = NULL;
	m_pMesh                    = NULL;
	m_pMeshMaterials           = NULL;
	m_pCrossHairTexture        = NULL;
	m_NumMaterials             = 0;
	m_BasicShader              = 0;
}

CTechniqueApplication::~CTechniqueApplication()
{
}

BOOL CTechniqueApplication::PostInitialize()
{	
	D3DCAPS8 Caps;
	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);
	if (Caps.VertexShaderVersion == D3DVS_VERSION(1,1))
	{
		if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
						  D3DCREATE_HARDWARE_VERTEXPROCESSING)))
			return FALSE;
	}
	else
	{
		if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
						  D3DCREATE_SOFTWARE_VERTEXPROCESSING)))
			return FALSE;
	}

	//Do the basic camera positioning, etc.
	SetupDevice();
	
	//Load the mesh object
	LoadMesh();

	//Create the buffers we're actually going to use
	ExtractBuffers();

	if (FAILED(CreateStencilVertexBuffer()))
		return FALSE;

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, 
		                                 "..\\media\\CrossHair.dds",
										 &m_pCrossHairTexture)))
		return FALSE;


	return TRUE;
}

void CTechniqueApplication::Render()
{
	//Set the eye position
	D3DXVECTOR4 EyePos(0.0, 60.0f, -60.0f, 0.0f);

	//Set the view matrix based on the position above.
	D3DXMatrixLookAtLH(&m_ViewMatrix, &(D3DXVECTOR3)EyePos,
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	//Set the current light shader
	m_pD3DDevice->SetVertexShader(m_BasicShader);


    //The following matrix operations will transform the mesh
	D3DXMATRIX Rotation;
    D3DXMATRIX Translation;

	//These matrices will rotate and translate the mesh
	D3DXMatrixRotationY(&Rotation, (float)GetTickCount() / 1000.0f);
	D3DXMatrixTranslation(&Translation, 0.0f, 5.0f, 0.0f);
	m_WorldMatrix = Rotation * Translation;

	//Set some lighting constants
	D3DXVECTOR4 Ambient    (0.1,  0.1f, 0.1f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(5, &Ambient, 1);

	//The light is moving side to side over the model.
	float Time = (float)GetTickCount() / 1000.0f;
	D3DXVECTOR4 LightPosition(150.0f * sin(Time), 70.0f, 0.0f, 1.0f);

	//Set the light direction based on what was computed for the shadow
	D3DXVECTOR4 LightDir = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f) - LightPosition;
	D3DXMATRIX InverseWorld;
	D3DXMatrixInverse(&InverseWorld, NULL, &m_WorldMatrix);
	D3DXVec4Transform(&LightDir, &LightDir, &InverseWorld);
	D3DXVec4Normalize(&LightDir, &LightDir);
	m_pD3DDevice->SetVertexShaderConstant(4, &LightDir, 1);


	//Create the concatenated transformation matrix
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * 
				              m_ProjectionMatrix;

	//Get the transpose
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);

	//Pass the transposed matrix to the shader
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);


	//The code above has set everything up for the rendering. First, render
	//the usual scene with the usual stuff.
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    								   m_pMesh->GetNumVertices(), 0,
									   m_pMesh->GetNumFaces());

	//Now, change the viewport and do it again...

	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	//First, save the viewport.
	D3DVIEWPORT8 NormalViewport;
	m_pD3DDevice->GetViewport(&NormalViewport);

	//Create a new viewport in the lower right
	D3DVIEWPORT8 ZoomViewport;
	ZoomViewport.X = 400;
	ZoomViewport.Y = 300;
	ZoomViewport.Width  = 200;
	ZoomViewport.Height = 150;
	ZoomViewport.MinZ = 0.0f;
	ZoomViewport.MaxZ = 1.0f;
	m_pD3DDevice->SetViewport(&ZoomViewport);

	//Change the projection matrix and reset the shader matrix accordingly.
	//The new projection matrix has a much smaller field of view than the 
	//original matrix. This creates the zoom effect.
	D3DXMATRIX ZoomProjection;
	D3DXMatrixPerspectiveFovLH(&ZoomProjection,
					D3DX_PI / 32, 1.5f, 1.0f, 1000.0f);

	ShaderMatrix = m_WorldMatrix * m_ViewMatrix * ZoomProjection;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	//Draw the stencil circle. Set it up so that everything passes and
	//the new pixels increment the stencil buffer
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE,   TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,     D3DCMP_ALWAYS);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);

	//Set it up so that nothing is drawn to the color buffer
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//Draw the actual circle
	m_pD3DDevice->SetVertexShader(D3DFVF_STENCILVERTEX);
	m_pD3DDevice->SetStreamSource(0, m_pStencilVertexBuffer, sizeof(STENCIL_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, CIRCLE_RES - 2);

	//Set most things back to normal
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   FALSE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	//Only draw if the value is greater than the ref value (zero)
	m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,  D3DCMP_LESS);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILREF,   0);

	m_pD3DDevice->SetVertexShader(m_BasicShader);
	m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    							   m_pMesh->GetNumVertices(), 0,
								   m_pMesh->GetNumFaces());

	//disable stenciling
	m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE,   FALSE);

	//Draw the crosshairs
	m_pD3DDevice->SetTexture(0, m_pCrossHairTexture);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pD3DDevice->SetVertexShader(D3DFVF_STENCILVERTEX);
	m_pD3DDevice->SetStreamSource(0, m_pStencilVertexBuffer, sizeof(STENCIL_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, CIRCLE_RES, 2);
	m_pD3DDevice->SetTexture(0, NULL);

	//Restore modes
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   FALSE);

	//Restore the old viewport
	m_pD3DDevice->SetViewport(&NormalViewport);

}

HRESULT CTechniqueApplication::LoadMesh()
{
	LPD3DXBUFFER pD3DXMtrlBuffer;
	LPD3DXMESH   pOriginalMesh;

    //Load and initialize the mesh. This is a repeat of the code
	//from Chapter 10.
	if(FAILED(D3DXLoadMeshFromX("..\\media\\fattorus.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, &pD3DXMtrlBuffer,
								&m_NumMaterials, &pOriginalMesh)))
        return FALSE;

	D3DXMATERIAL* d3dxMaterials = 
		(D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

    m_pMeshMaterials = new D3DMATERIAL8[m_NumMaterials];

    for(long MatCount = 0; MatCount < m_NumMaterials; MatCount++)
    {
		m_pMeshMaterials[MatCount] = d3dxMaterials[MatCount].MatD3D;
        m_pMeshMaterials[MatCount].Ambient = 
								m_pMeshMaterials[MatCount].Diffuse;
    }

    pD3DXMtrlBuffer->Release();

	//This is new. If the FVF doesn't match, clone the mesh and
	//create one that does. Then, release the loaded mesh. If the 
	//FVF does match, set the member mesh and move on.
	if (pOriginalMesh->GetFVF() != D3DFVF_MESHVERTEX)
	{
		pOriginalMesh->CloneMeshFVF(D3DXMESH_MANAGED,
									D3DFVF_MESHVERTEX,
                                    m_pD3DDevice, &m_pMesh);
		
		pOriginalMesh->Release();
		pOriginalMesh = NULL;
	}
	else
		m_pMesh = pOriginalMesh;

	return S_OK;
}

BOOL CTechniqueApplication::PreReset()
{
	//Delete the shaders
	m_pD3DDevice->DeleteVertexShader(m_BasicShader);

	return TRUE;
}

BOOL CTechniqueApplication::PostReset()
{
	SetupDevice();

	//Recreate the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	return TRUE;
}


BOOL CTechniqueApplication::PreTerminate()
{
	//Delete the shaders
	m_pD3DDevice->DeleteVertexShader(m_BasicShader);
	
	//Clean up
	if (m_pStencilVertexBuffer)
	{
		m_pStencilVertexBuffer->Release();
		m_pStencilVertexBuffer = NULL;
	}

	if (m_pCrossHairTexture)
	{
		m_pCrossHairTexture->Release();
		m_pCrossHairTexture = NULL;
	}

	if (m_pMeshVertexBuffer)
	{
		m_pMeshVertexBuffer->Release();
		m_pMeshVertexBuffer = NULL;
	}

	if (m_pMeshIndexBuffer)
	{
		m_pMeshIndexBuffer->Release();
		m_pMeshIndexBuffer  = NULL;
	}

	if (m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	if (m_pMeshMaterials)
	{
		delete m_pMeshMaterials;
		m_pMeshMaterials = NULL;
	}

	return TRUE;
}


void CTechniqueApplication::SetupDevice()
{
	//Do all the basic setup
	RECT WindowRect;
	GetClientRect(m_hWnd, &WindowRect);
	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix,
					D3DX_PI / 4,
					(float)(WindowRect.right - WindowRect.left) / 
					(float)(WindowRect.bottom - WindowRect.top),
				    1.0f, 1000.0f);

	D3DXMatrixIdentity(&m_WorldMatrix);
}

HRESULT CTechniqueApplication::ExtractBuffers()
{
	//Get the buffers
	m_pMesh->GetVertexBuffer(&m_pMeshVertexBuffer);
	m_pMesh->GetIndexBuffer(&m_pMeshIndexBuffer);

	MESH_VERTEX *pMeshVertices;
	short       *pIndices;
	DWORD       *pAttribs;

	//Lock the vertex buffer, but allow writing.
	m_pMeshVertexBuffer->Lock(0, 
		                      m_pMesh->GetNumVertices() * sizeof(MESH_VERTEX),
		                      (BYTE **)&pMeshVertices, 0);


	//We only need to read the indices
	m_pMeshIndexBuffer->Lock(0, 3 * m_pMesh->GetNumFaces() * sizeof(short),
	                         (BYTE **)&pIndices, D3DLOCK_READONLY);

	//The attribute buffer maps the materials to each face.
	m_pMesh->LockAttributeBuffer(D3DLOCK_READONLY, &pAttribs);

	//Loop through each face and set the vertex color based on the material.
	//This is a pretty simple example, but you could also use this to preload
	//other data, such as converting colors to data that the vertex shader
	//may use in computations.
	for (long Face = 0; Face < m_pMesh->GetNumFaces(); Face++)
	{
		D3DXCOLOR Diffuse = (D3DXCOLOR)m_pMeshMaterials[pAttribs[Face]].Diffuse;

		pMeshVertices[pIndices[Face * 3 + 0]].color = Diffuse;
		pMeshVertices[pIndices[Face * 3 + 1]].color = Diffuse;
		pMeshVertices[pIndices[Face * 3 + 2]].color = Diffuse;
	}

	//Give back all of our buffers.
	m_pMeshVertexBuffer->Unlock();
	m_pMeshIndexBuffer->Unlock();
	m_pMesh->UnlockAttributeBuffer();

	return S_OK;
}

HRESULT CTechniqueApplication::CreateShaders()
{
	//Set up the declaration to match the FVF and to
	//read from stream zero.
	DWORD Declaration[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(D3DVSDE_POSITION,D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_NORMAL,  D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
		D3DVSD_END()
	};

	
	ID3DXBuffer* pShaderBuffer;
	ID3DXBuffer* pShaderErrors;

	//Assemble and create the first shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\Basic.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_BasicShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	return S_OK;
}

void CTechniqueApplication::PreRender()
{
	//Clear the device
	m_pD3DDevice->Clear(0, NULL,
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
						D3DCOLOR_XRGB(100, 100, 200), 1.0f, 0);

	//Call BeginScene to set up the device
	m_pD3DDevice->BeginScene();
		
	return;
}


HRESULT CTechniqueApplication::CreateStencilVertexBuffer()
{
	//Create as managed so we don't have to worry about recreating it
	//if the device is lost.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(NUM_VERTICES * sizeof(STENCIL_VERTEX), 
		                                        0, D3DFVF_STENCILVERTEX,
									            D3DPOOL_MANAGED,
												&m_pStencilVertexBuffer)))
		return E_FAIL;

	//Create a set of 4 vertices for the plane
	STENCIL_VERTEX *pVertices;

	//Lock the vertex buffer, but allow writing.
	m_pStencilVertexBuffer->Lock(0, NUM_VERTICES * sizeof(STENCIL_VERTEX),
		                         (BYTE **)&pVertices, 0);

	//We'll use a fan. Define the center of the circle. Presently, the viewport
	//is 200 by 150, so the center is 500, 375
	pVertices[0].rhw = 1.0f;
	pVertices[0].x = 500.0f;
	pVertices[0].y = 375.0f;
	pVertices[0].z = 1.0f;

	//Now, define all the surrounding vertices except for the last one.
	//Make the circle with a radius of 70.0
	for (long CirclePoint = 1; CirclePoint < CIRCLE_RES - 1; CirclePoint++)
	{
		float Angle = 2.0f * D3DX_PI * (float)(CirclePoint - 1) / (CIRCLE_RES - 2);

		pVertices[CirclePoint].x   = 500.0f + (70.0f * cos(Angle));
		pVertices[CirclePoint].y   = 375.0f + (70.0f * sin(Angle));
		pVertices[CirclePoint].z   = 1.0f;
		pVertices[CirclePoint].rhw = 1.0f;
	}

	//Define the last one 
	pVertices[CirclePoint].x   = 570.0f;
	pVertices[CirclePoint].y   = 375.0f;
	pVertices[CirclePoint].z   = 1.0f;
	pVertices[CirclePoint].rhw = 1.0f;

	//Define the final four points that make up the crosshair image.
	//These values are all "empirical"
	pVertices[CirclePoint + 1].x     = 422.0f;
	pVertices[CirclePoint + 1].y     = 298.0f;
	pVertices[CirclePoint + 1].u     = 0.0f;
	pVertices[CirclePoint + 1].v     = 0.0f;
	pVertices[CirclePoint + 2].x     = 578.0f;
	pVertices[CirclePoint + 2].y     = 298.0f;
	pVertices[CirclePoint + 2].u     = 1.0f;
	pVertices[CirclePoint + 2].v     = 0.0f;
	pVertices[CirclePoint + 3].x     = 422.0f;
	pVertices[CirclePoint + 3].y     = 452.0f;
	pVertices[CirclePoint + 3].u     = 0.0f;
	pVertices[CirclePoint + 3].v     = 1.0f;
	pVertices[CirclePoint + 4].x     = 578.0f;
	pVertices[CirclePoint + 4].y     = 452.0f;
	pVertices[CirclePoint + 4].u     = 1.0f;
	pVertices[CirclePoint + 4].v     = 1.0f;

	pVertices[CirclePoint + 1].z = pVertices[CirclePoint + 2].z =
	pVertices[CirclePoint + 3].z = pVertices[CirclePoint + 4].z	= 1.0f;

	pVertices[CirclePoint + 1].rhw = pVertices[CirclePoint + 2].rhw =
	pVertices[CirclePoint + 3].rhw = pVertices[CirclePoint + 4].rhw	= 1.0f;

	m_pStencilVertexBuffer->Unlock();

	return S_OK;
}

