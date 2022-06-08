//AUTHOR Oskar Schramm 2k22

#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

class RenderTexture
{
public:
	RenderTexture();
	~RenderTexture();

	bool Init(ID3D11Device* aDevice, const int aTextureWidth, const int aTextureHeight);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext* aDeviceContext, ID3D11DepthStencilView* aDepthStencilView);
	void ClearRenderTarget(ID3D11DeviceContext* aDeviceContext, ID3D11DepthStencilView* aDepthStencilView, const float r, const float g, const float b, const float a);
	inline ID3D11ShaderResourceView* GetShaderResourceView() { return myShaderResourceView; };

private:
	ID3D11Texture2D* myRenderTexture;
	ID3D11RenderTargetView* myRenderTargetView;
	ID3D11ShaderResourceView* myShaderResourceView;
};