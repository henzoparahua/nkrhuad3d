#include "../header/d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
	m_videoCardMemory = 0;
	memset(m_videoCardDescription, 0, sizeof(m_videoCardDescription));
}

D3DClass::D3DClass(const D3DClass& other)
{

}

D3DClass::~D3DClass()
{

}

//	Setting up the direct3D stuff for dx11
bool D3DClass::Initialize(int scrnWidth, int scrnHeight,
	bool vsync, HWND hwnd, bool fscreen, float scrnDepth, float scrnNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, scrnAspect;

	m_vsync_enabled = vsync;

	//	Before initializing Direct3D, we have to get the refresh rate from the video/monitor.
	//	Each computer may be slightly different so we will need to query for that information.
	//	We query the DXGI (DirectX Graphics Infrastructure) to get the refresh rate of the monitor.

	//	1. Create a DXGIFactory object:
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//	2. The factory enumerates the adapters (graphics cards) on the system:
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//	3. The adapter enumerates the outputs (monitors) connected to the adapter:
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// 4. Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 5. Create a list to hold all the possible display modes for this monitor/video card combinations.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 6. Fill the display mode list sctructures:
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// 7. Now, go through all the display modes and find the one that matches the screen width and height.
	// When a good match is found, store the numerator and denominator of the refresh rate of that monitor.
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (signed int)scrnWidth)
		{ 
			if (displayModeList[i].Height == (unsigned int)scrnHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// At last, we will retrieve using the adapter of the video card and the amount of video memory.

	// 8. Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// 9. Store the dedicated video card memory in megabytes:
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 10. Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// 11. We can release the structures and interfaces used to get that information. 
	
	// Release the display mode list:
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output:
	adapterOutput->Release();
	adapterOutput = 0;
	
	// Release the adapter:
	adapter->Release();
	adapter = 0;
	
	// Release the factory:
	factory->Release();
	factory = 0;

	//  12. Now that we have the refresh rate from the system, we can finally start the DirectX Initialization.
	
	//		The first thing we'll do is fill out the description of the swap chain.
	
	//		Swap Chain is the front and back buffer to which the graphics will be drawn. 
	//		Generally you use a single back buffer, do all ur drawing to it, and then swap it to the front buffer
	//		which then displays on the user's screen. Thats why its called Swap Chain.

	//	Initialize the Swap Chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//	Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	//	Set the Width and Height of the Back Buffer.
	swapChainDesc.BufferDesc.Width = scrnWidth;
	swapChainDesc.BufferDesc.Height = scrnHeight;

	//	Set regular 32-bit surface for the Back Buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//	Then we set the description of the refresh rate for the Swap Chain. 
	
	//	The Refresh Rate is how many times a second it draws the Back Buffer to the Front Buffer. 
	
	//	If the vsync is set to true in our applicationclass.h header then this will lock our refresh rate to the System 
	//	Settings (for example, 60hz). That means it will only draw the screen 60 times a second (or higher if the system 
	//	refresh rate is more than 60).
	
	//	If we set vsync to false, it will draw the screen as many times as possible, but it can cause some visual artifacts.

	
	//	Set the refresh rate of the Back Buffer.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else 
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//	Set the usage of the Back Buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//	Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	//	Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//	Set to Full Screen or Windowed Mode:
	if (fscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else 
	{
		swapChainDesc.Windowed = true;
	}
	
	//	Set the scan line ordering the scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//	Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	//	This variable below tells DirectX what version we plan to use. Here,we can use multiple versions or running on lower end hardware.
	featureLevel = D3D_FEATURE_LEVEL_10_0;

	//	Now that the Swap Chain description and feature level have been filled, we can truly create the Swap Chain,
	//	the Direct3D device and the Direct3D Device Context. We will use the Device and Device COntext almost for eveything.
	
	//	If the one who open the executable does not have a proper DirectX 11 Video Card, this function call will fail.
	
	//	Create the Swap Chain, Direct3D Device and Device Context:
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, 
		&swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}
	
	//	Sometimes this call to create the Device will fail if the primary Video Card is not compatible with DirectX 11.
	//	Some machines may have the primary card as a DirectX 10 and the secondary as a DirectX 11 Video Card.
	//	Also, hybrid graphics card work that way with the primary being the low power Intel Card, and the secondary
	//	being the high power Nvidia card. To get around this you will need to not use the default device and instead
	//	enumerate all the video cards in the machine and have the user choose which one to use and then specify that card.

	//	TODO: enumerate the video cards and pick the best for DirectX 11.

	//	Now, back to the Swap Chain, we need to get a pointer to the Back Buffer and then attach to the Swap Chain.
	//	We'll use the CreateRenderTargetView function to attach the Back Buffer to our Swap Chain.

	//	Get the pointer to the Back Buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	//	Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);\
	if (FAILED(result))
	{
		return false;
	}

	//	Release the pointer to the Back Bufgfer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;
	
	//	We need to set up a depth buffer description. We'll use this to create a depth buffer so that our polygons
	//	can be rendered properly in 3D space. At the same time, we will attach a stencil buffer to our depth buffer.
	//	The stencil can be used to achieve effects such as motion blur, volumetric shadows and other things.

	//	Initialize the description of the Depth Buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//	Set up the description of the Depth Buffer.
	depthBufferDesc.Width = scrnWidth;
	depthBufferDesc.Height = scrnHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//	Now we create the Depth/Stencil Buffer using that description.
	//	U'll notice we use the CreateTexture2D function to make the buffers, hence the buffer is just 2D.
	//	The reasion for this is that once ur polygons are sorted and then rasterized, they just end up being
	//	colored pixels in this 2D buffer. Then this 2D buffer is drawn to the screen.

	//	Create the texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//	Now we need to setup the depth description. 
	//	This allow us to control what type of depth test Direct3D will do for each pixel.
	
	//	Initialize the description of the stencil state:
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//	Set up the description of the Stencil State:
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//	Stencil operations if pixel is front-facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//	Stencil operations if pixel is back-facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//	With the description filled out we can now create a depth stencil state.
	
	//	Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	//	The next thing we need to create is the description of the view of rthe depth stencil buffer.
	//	We do this so that Direct3D knows to use the depth buffer as a depth stencil texture.
	//	After filling out the description we then call the function CreateDepthStencilView to create it.

	//	Initialize the Depth Stencil View.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//	Set up the Depth Stencil View Description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//	Create the Depth Stencil View.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	//	Now, call OMSetRenderTargets. This will bind the render target view and the depth stencil buffer to the output render
	//	pipeline. This way the graphics that the pipeline renders will get drawn to our back buffer that we previously created.
	//	With the graphics written to the back buffer we can then swap it to the front and display our graphics on the screen.
	
	//	Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//	Now that the render targets are setup we can continue on to some extra functions that will give us more control over
	//	our scenes. First thing is we'll create is a rasterizer state. This will gibe us control over how polygons are 
	//	rendered. We can do things like make our scenes render in wireframe mode or have DirectX draw both the front and back 
	//	faces of polygons. By defaukt, DirectX already has a rasterizer state set up and working the exact same as the below.

	//	Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//	Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	//	Now set the rasterizer state.
	m_deviceContext->RSSetState(m_rasterState);

	//	Setting up the viewport, so that Direct3D can map clip space coordinates to render target space.
	//	We set this to be the entire size of the window.

	//	Setup the viewport for rendering.
	m_viewport.Width = (float)scrnWidth;
	m_viewport.Height = (float)scrnHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	//	Create the viewport.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	//	The projection matrix is used to translate the 3D scene into the 2D viewport space that we previously created.
	//	We will setup this now, to keep a copy of this matrix so that will be used to render our scenes.

	//	Setup the projection matrix.
	fieldOfView = 3.141592654f / 4.0f;
	scrnAspect = (float)scrnWidth / (float)scrnHeight;

	//	Create the projection matrix for 3D rendering.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, scrnAspect, scrnNear, scrnDepth);

	//	We will also create another matrix called the World Matrix. This matrix is used to convert the vertices of our 
	//	objects into vertices in the 3D scene. This matrix will also be used to rotate, translate, and scale our objects
	//	in 3D space. From the start we will just initialize the matrix to the identity matrix and keep a copy of it in
	//	this object. The copy will be needed to be passed to the shaders for rendering also.
	
	//	Initialize the World Matrix to the Identity Matrix
	m_worldMatrix = XMMatrixIdentity();


	//	This is where generally we create a view matrix. The View Matrix is used to calculate the position of where
	//	we are looking at the scene from. Think of it as a camera and you only view the scene through this camera.
	
	//	And finally, we will setup in the initialize function a orthographic projection matrix. This matrix is used for
	//	Rendering 2D elements like user interface on the screen allowing uis to skip the 3D rendering.

	//	Create an orthographic matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)scrnWidth, (float)scrnHeight, scrnNear, scrnDepth);

	return true;
}

void D3DClass::Shutdown()
{
	//	Before shutting down, set to windowed mode or when you release the swap chain it will thrown an exception.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

//	In D3DClass we have a couple of helper functions. First two are BeginScene and EndScene, which will be called 
//	whenever we are going to draw a new 3D scene at the beginning of each frame. All it does is initializes the buffers
//	so they are blank and ready to be drawn to. The other function is EndScene, it tells the swap chain to display
//	our 3D scene once all the drawing has completed at the end of each frame.

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	//	Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//	Clear the Back Buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//	Create the Depth Buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	//	Present the Back Buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		//	Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		//	Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}

//	Helper functions for getting pointers.

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

//	The next three helper functions give copies of the projection, world and orthographic matrices to calling functions.
//	Most shaders will need these matrices for rendering so there needed to be an easy way for outside objects to get a copy.

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

//	The helper function returns by reference the name of the video card and the amount of video memory. Knowing the video
//	card name can help in debugging on different configurations.

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void D3DClass::ResetViewport()
{
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}