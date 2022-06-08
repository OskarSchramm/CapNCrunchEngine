//AUTHOR Oskar Schramm 2k22

#include "D3D11Class.h"

D3D11Class::D3D11Class()
	: mySwapChain(nullptr), myDevice(nullptr), myDeviceContext(nullptr), myRenderTargetView(nullptr),
	myDepthStencilBuffer(nullptr), myDepthStencilState(nullptr), myDepthStencilView(nullptr)
{}

D3D11Class::~D3D11Class(){}

bool D3D11Class::Initialize(int aScreenWidth, int aScreenHeight, bool aVsync, HWND aHWND, bool aFullscreen)
{
	unsigned int numerator, denominator;
	myVsyncEnabled = aVsync;

	if (!ReadAdapter(numerator, denominator, aScreenWidth))
		return false;

	if (!CreateDeviceAndSwapChain(&aHWND, aFullscreen, numerator, denominator))
		return false;

	if (!CreateViewPort(aScreenWidth, aScreenHeight))
		return false;

	if (!CreateDepthBufferAndStencil(myDepthStencilState, aScreenWidth, aScreenHeight))
		return false;

	return true;
}

bool D3D11Class::CreateDepthBufferAndStencil(ID3D11DepthStencilState* anOutDepthStencilState, const UINT aWidth, const UINT aHeight)
{
	HRESULT result;

	//DepthBuffer
	ID3D11Texture2D* depthBufferTexture;
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = aWidth;
	depthBufferDesc.Height = aHeight;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	result = myDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthBufferTexture);
	if (FAILED(result))
		return false;

	result = myDevice->CreateDepthStencilView(depthBufferTexture, nullptr, &myDepthStencilView);
	if (FAILED(result))
		return false;

	//DepthStencil 
	D3D11_DEPTH_STENCIL_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.DepthEnable = true;
	depthStencilViewDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilViewDesc.DepthFunc = D3D11_COMPARISON_LESS;

	result = myDevice->CreateDepthStencilState(&depthStencilViewDesc, &anOutDepthStencilState);
	if (FAILED(result)) return false;

	depthBufferTexture->Release();

	myDeviceContext->OMSetRenderTargets(1, &myRenderTargetView, myDepthStencilView);
	myDeviceContext->OMSetDepthStencilState(myDepthStencilState, 1);

	return true;
}

bool D3D11Class::ReadAdapter(UINT& aNumerator, UINT& aDenominator, int aScreenWidth)
{
	HRESULT result;

	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;

	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;

	unsigned int stringLength, numModes;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
		return false;

	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
		return false;

	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
		return false;

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
		return false;

	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
		return false;

	for (size_t i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)aScreenWidth)
		{
			aNumerator = displayModeList[i].RefreshRate.Numerator;
			aDenominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
		return false;

	//In megabytes
	myVideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//This is weird in 64 bits but works in 32
	//error = wcstombs_s(& stringLength, myVideoCardDesc, 128, adapterDesc.Description, 256);
	//if (error != 0)
	//	return false;

	//Cleanup
	delete[] displayModeList;
	displayModeList = nullptr;

	adapterOutput->Release();
	adapterOutput = nullptr;

	adapter->Release();
	adapter = nullptr;

	factory->Release();
	factory = nullptr;

	return true;
}

bool D3D11Class::CreateDeviceAndSwapChain(HWND* aWindowHandle, const bool aFullscreen, UINT numerator, UINT denominator)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = *aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (myVsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	if (aFullscreen)
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&myDevice,
		nullptr,
		&myDeviceContext);

	if (FAILED(result))
		return false;

	return true;
}

bool D3D11Class::CreateViewPort(int& aWidth, int& aHeight)
{
	HRESULT result;

	ID3D11Texture2D* backBufferTexture;
	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result)) return false;

	result = myDevice->CreateRenderTargetView(backBufferTexture, nullptr, &myRenderTargetView);
	if (FAILED(result)) return false;

	//Viewport
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = aWidth;
	textureDesc.Height = aHeight;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();

	aWidth = static_cast<float>(textureDesc.Width);
	aHeight = static_cast<float>(textureDesc.Height);

	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = aWidth;
	viewport.Height = aHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	myDeviceContext->RSSetViewports(1, &viewport);

	return true;
}

void D3D11Class::Shutdown()
{
	//B4 shutting down, set swap chain to windowed to avoid exceptions
	if (mySwapChain)
		mySwapChain->SetFullscreenState(false, NULL);

	if (myDepthStencilView)
	{
		myDepthStencilView->Release();
		myDepthStencilView = 0;
	}

	if (myDepthStencilState)
	{
		myDepthStencilState->Release();
		myDepthStencilState = 0;
	}

	if (myDeviceContext)
	{
		myDeviceContext->Release();
		myDeviceContext = 0;
	}

	if (myDevice)
	{
		myDevice->Release();
		myDevice = 0;
	}

	if (mySwapChain)
	{
		mySwapChain->Release();
		mySwapChain = 0;
	}
}

void D3D11Class::BeginFrame(float aRed, float aGreen, float aBlue, float anAlpha)
{
	float color[4];

	color[0] = aRed;
	color[1] = aGreen;
	color[2] = aBlue;
	color[3] = anAlpha;

	myDeviceContext->ClearRenderTargetView(myRenderTargetView, color);
	myDeviceContext->ClearDepthStencilView(myDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D11Class::EndFrame()
{
	if (myVsyncEnabled)
	{
		mySwapChain->Present(1, 0);
	}
	else
	{
		mySwapChain->Present(0, 0);
	}
}

void D3D11Class::GetVideoCardInfo(char* aCardName, int& aVideoMemory)
{
	strcpy_s(aCardName, 128, myVideoCardDesc);
	aVideoMemory = myVideoCardMemory;
}