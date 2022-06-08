//AUTHOR Oskar Schramm 2k22

#include "RenderTexture.h"
#include <d3d11.h>

RenderTexture::RenderTexture()
	: myRenderTexture(nullptr), myRenderTargetView(nullptr), myShaderResourceView(nullptr)
{}

RenderTexture::~RenderTexture()
{}

bool RenderTexture::Init(ID3D11Device* aDevice, const int aTextureWidth, const int aTextureHeight)
{
	return true;
}

void RenderTexture::Shutdown()
{
	if (myShaderResourceView)
	{
		myShaderResourceView->Release();
		myShaderResourceView = nullptr;
	}
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* aDeviceContext, ID3D11DepthStencilView* aDepthStencilView)
{
	aDeviceContext->OMSetRenderTargets(1, &myRenderTargetView, aDepthStencilView);
}

void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* aDeviceContext, ID3D11DepthStencilView* aDepthStencilView, const float aRed, const float aGreen, const float aBlue, const float aAlpha)
{
	float color[4];

	color[0] = aRed;
	color[1] = aGreen;
	color[2] = aBlue;
	color[3] = aAlpha;

	aDeviceContext->ClearRenderTargetView(myRenderTargetView, color);

	aDeviceContext->ClearDepthStencilView(aDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}