//AUTHOR Oskar Schramm 2k22

#pragma once
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

class D3D11Class
{
public:
	D3D11Class();
	~D3D11Class();

	bool Initialize(int aScreenWidth, int aScreenHeight, bool aVsync, HWND aHWND, bool aFullscreen);
	void Shutdown();

	void BeginFrame(float, float, float, float);
	void EndFrame();

	inline ID3D11Device* GetDevice() const { return myDevice; }
	inline ID3D11DeviceContext* GetDeviceContext() const { return myDeviceContext; }

	void GetVideoCardInfo(char*, int&);
private:
	bool ReadAdapter(UINT& aNumerator, UINT& aDenominator, int aScreenWidth);
	bool CreateDeviceAndSwapChain(HWND* aWindowHandle, const bool aFullscreen, UINT numerator, UINT denominator);
	bool CreateViewPort(int& aWidth, int& aHeight);
	bool CreateDepthBufferAndStencil(ID3D11DepthStencilState* anOutDepthStencilState, const UINT aWidth, const UINT aHeight);

	bool myVsyncEnabled;
	int myVideoCardMemory;
	char myVideoCardDesc[128];

	IDXGISwapChain*          mySwapChain;
	ID3D11Device*		     myDevice;
	ID3D11DeviceContext*     myDeviceContext;
	ID3D11RenderTargetView*  myRenderTargetView;
	ID3D11Texture2D*		 myDepthStencilBuffer;
	ID3D11DepthStencilState* myDepthStencilState;
	ID3D11DepthStencilView*  myDepthStencilView;
};

