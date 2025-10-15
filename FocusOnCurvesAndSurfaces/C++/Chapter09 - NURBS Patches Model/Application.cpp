/***************************************************************
* Application.cpp                                              *
*                                                              *
* This file contains the implementation of the                 *
* CHostApplication class.									   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
* Changes:                                                     *
*	*  Added Direct3D device support to CHostApplication       *
*	*  Added several virtual functions to allow easy           *
*      subclassing  in later chapters.                         *
*   *  Removed previous comments and commented new code        *
***************************************************************/

#include "Executable.h"
#include "Application.h"

CHostApplication::CHostApplication()
{
	m_WindowWidth  = 640;
	m_WindowHeight = 480;
}

void CHostApplication::Go()
{
	WNDCLASSEX WindowClass = {sizeof(WNDCLASSEX), 
							  CS_CLASSDC,
							  EntryMessageHandler,
							  0,
							  0, 
							  GetModuleHandle(NULL),
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  "Host Application",
							  NULL};

	RegisterClassEx(&WindowClass);

    m_hWnd = CreateWindow("Host Application",
							"Host Application", 
							WS_OVERLAPPEDWINDOW,
							0,
							0,
							m_WindowWidth,
							m_WindowHeight,
							GetDesktopWindow(),
							NULL,
							WindowClass.hInstance,
							NULL);

	if(m_hWnd)
		ShowWindow(m_hWnd, SW_SHOW);

	//Initialize the Continue flag.
	m_Continue = TRUE;

	//Call the PreInitialize function to allow any children
	//to preset anything
	if (!PreInitialize())
		return;

	//Initialize the D3D object.
	InitializeD3D();

	//Call the PostIinitialize function to allow any children
	//to continue initializing anything
	if (!PostInitialize())
		return;

	MSG Message; 
	PeekMessage(&Message, 0, 0, 0, PM_REMOVE);

	while (Message.message != WM_QUIT && m_Continue)
    {
		//Call PreRender first
		PreRender();

		//Call the main render function
		Render();

		//Present the backbuffer contents to the display.  If
		//we lose the device, let the device class get it back.
		if (D3DERR_DEVICELOST == PostRender())
		{
			//do any pre-cleanup
			PreReset();

			//Let the device class reset the device
			RestoreDevice();

			//Tell the rest of the app that this has happened
			PostReset();
		}


		TranslateMessage(&Message);
        DispatchMessage(&Message);
		PeekMessage(&Message, 0, 0, 0, PM_REMOVE);

		//If the child class returns false, stop the program.
		m_Continue = HandleMessage(&Message);
    }

	//Call the PreTerminate function in case there is any 
	//pre-cleanup to do.
	PreTerminate();

	//Destroy the Device
	DestroyDevice();

	//Call the PostTerminate function in case there is any 
	//post-cleanup to do.
	PostTerminate();
}

//The prerender function handles everything that happens before
//rendering, such as clearing the buffers.  If an application
//implements its own PreRender function, that function MUST
//call BeginScene.
void CHostApplication::PreRender()
{
	//Clear the device
	m_pD3DDevice->Clear(0, NULL,
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						D3DCOLOR_XRGB(210, 200, 180), 1.0f, 0);

	//Call BeginScene to set up the device
	m_pD3DDevice->BeginScene();
		
	return;
}

//The PostRender function stops the rendering process.  If
//another application implements this function, it MUST
//call EndScene. It returns the result of Present so that
//the device restoration function knows what to do...
HRESULT CHostApplication::PostRender()
{
	// End the scene
	m_pD3DDevice->EndScene();

	return m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}


CHostApplication::~CHostApplication()
{
	//Release the D3D object
	if (m_pD3D)
		m_pD3D->Release();

	m_pD3D = NULL;
}

LRESULT WINAPI CHostApplication::MessageHandler(HWND hWnd, 
												UINT Message, 
												WPARAM wParam,
												LPARAM lParam)
{
	switch(Message)
    {
		case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
	return DefWindowProc(hWnd, Message, wParam, lParam);
}

HRESULT CHostApplication::InitializeD3D()
{
	//Create the actual D3D object.
	m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);

	//Set the default device value to NULL
	m_pD3DDevice = NULL;

	//Return a default value
	return S_OK;
}

HRESULT CHostApplication::CreateDevice(
			D3DDEVICE_CREATION_PARAMETERS *pCreateParms,
			D3DPRESENT_PARAMETERS         *pPresentParms)
{
	//Save the parameter blocks
	memcpy(&m_CreationParameters, pCreateParms, 
		   sizeof(D3DDEVICE_CREATION_PARAMETERS));
	memcpy(&m_PresentParameters, pPresentParms,
		   sizeof(D3DPRESENT_PARAMETERS));

	//Create the device
	return m_pD3D->CreateDevice(pCreateParms->AdapterOrdinal, 
		                        pCreateParms->DeviceType,
						        pCreateParms->hFocusWindow,
						        pCreateParms->BehaviorFlags,
						        pPresentParms,
						        &m_pD3DDevice);
}

HRESULT CHostApplication::DestroyDevice()
{
	//Release the D3D Device
	if (m_pD3DDevice)
		m_pD3DDevice->Release();

	m_pD3DDevice = NULL;

	//return a default value
	return S_OK;
}

long CHostApplication::EnumerateModes(D3DDISPLAYMODE *pModes,
									  long ModeCount)
{
	//Get the actual number of available modes
	long Count = m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT);

	//If they asked for more than what is available, only give
	//them what is available.
	if (ModeCount > Count)
		ModeCount = Count;

	//Loop through the available modes, filling the array until
	//it is full.
	for (long ModeIndex = 0; ModeIndex < ModeCount; ModeIndex++)
	{
		m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, ModeIndex, 
							     &(pModes[ModeIndex]));
	}

	//Return the actual number available, in case they want to
	//ask for more.
	return Count;
}

HRESULT CHostApplication::RestoreDevice()
{
	HRESULT Result = m_pD3DDevice->TestCooperativeLevel();

	//If the device is lost, enter a loop waiting for
	//it to be restored.
	while(Result == D3DERR_DEVICELOST)
	{
		//Keep testing the level until it says we 
		//can reset.
		while(Result != D3DERR_DEVICENOTRESET)
		{
			//Give up control to other applications
			Sleep(1000);

			//Pump messages in order to respond to messages
			//that may lead to restoration.
			MSG Message;
			PeekMessage(&Message, 0, 0, 0, PM_REMOVE);
			TranslateMessage(&Message);
			DispatchMessage(&Message);

			//Check to see if things are ready to be reset
			Result = m_pD3DDevice->TestCooperativeLevel();
		}

		//Reset the device using the saved parameters
		if (FAILED(m_pD3DDevice->Reset(&m_PresentParameters)))
			Result = D3DERR_DEVICELOST;
	}	

	return S_OK;
}

HRESULT CHostApplication::EasyCreateWindowed(HWND WindowHandle, 
							                 D3DDEVTYPE DeviceType, 
							                 DWORD Behavior)
{
	D3DDISPLAYMODE CurrentMode;
	if (SUCCEEDED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,
								                &CurrentMode)))
	{
		//Since this is a convenience function for the developer,
		//make sure your settings make sense for your hardware.
		//These settings work well for a geForce3.
		ZeroMemory(&m_PresentParameters, 
				   sizeof(D3DPRESENT_PARAMETERS));
		m_PresentParameters.Windowed = TRUE;
		m_PresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_PresentParameters.BackBufferFormat = CurrentMode.Format;
		m_PresentParameters.EnableAutoDepthStencil = TRUE;
		m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;

		m_CreationParameters.AdapterOrdinal = 
											D3DADAPTER_DEFAULT;
		m_CreationParameters.DeviceType     = DeviceType;
		m_CreationParameters.hFocusWindow   = WindowHandle;
		m_CreationParameters.BehaviorFlags  = Behavior;

		HRESULT hr = CreateDevice(&m_CreationParameters, 
							      &m_PresentParameters);
		if (FAILED(hr))
		{
			//OK... try SOFTWARE vertex processing
			m_CreationParameters.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			hr = CreateDevice(&m_CreationParameters, &m_PresentParameters);
			
			if (FAILED(hr))
			{
				m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D16;
				hr = CreateDevice(&m_CreationParameters, &m_PresentParameters);

				if (FAILED(hr))
				{
					MessageBox(NULL, "I was unable to create a valid device.\nIn most cases, this means that your card does not support the creation parameters I have chosen.\nIt might be best to experiment to find parameters that work with your card,\nor change the call to EasyCreateWindowed to use the REF device.", NULL, MB_OK);
				}
			}
		}

		return hr;
	}

	//if we got this far, we've failed
	return E_FAIL;
}

HRESULT CHostApplication::EasyCreateFullScreen(D3DDISPLAYMODE *pMode,
							                   D3DDEVTYPE DeviceType, 
							                   DWORD Behavior)
{
	//Since this is a convenience function for the developer,
	//make sure your settings make sense for your hardware.
	//These settings work well for a geForce3.
	ZeroMemory(&m_PresentParameters, 
			   sizeof(D3DPRESENT_PARAMETERS));
	m_PresentParameters.Windowed = FALSE;
	m_PresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_PresentParameters.BackBufferWidth = pMode->Width;
	m_PresentParameters.BackBufferHeight = pMode->Height;
	m_PresentParameters.BackBufferFormat = pMode->Format;
	m_PresentParameters.FullScreen_RefreshRateInHz = 
										pMode->RefreshRate;
	m_PresentParameters.EnableAutoDepthStencil = TRUE;
	m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;

	m_CreationParameters.AdapterOrdinal = 
										D3DADAPTER_DEFAULT;
	m_CreationParameters.DeviceType     = DeviceType;
	m_CreationParameters.BehaviorFlags  = Behavior;
	m_CreationParameters.hFocusWindow   = m_hWnd;

	return CreateDevice(&m_CreationParameters, 
						&m_PresentParameters);
}

//This is our overridable message processor.  We handle the
//escape key as an example of how to use this function
BOOL CHostApplication::HandleMessage(MSG *pMessage)
{
	if (pMessage->message == WM_KEYDOWN && pMessage->wParam == VK_ESCAPE)
		return FALSE;

	return TRUE;
}

//These are placeholders for functions that can be implemented
//by other classes.  They will be discussed more in later 
//code samples.
//If any of these functions return FALSE, the application will
//stop.
void CHostApplication::Render(){}
BOOL CHostApplication::PreInitialize(){return TRUE;}
BOOL CHostApplication::PreTerminate(){return TRUE;}
BOOL CHostApplication::PostInitialize(){return TRUE;}
BOOL CHostApplication::PostTerminate(){return TRUE;}
BOOL CHostApplication::PreReset(){return TRUE;}
BOOL CHostApplication::PostReset(){return TRUE;}



