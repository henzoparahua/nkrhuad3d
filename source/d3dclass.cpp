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
}