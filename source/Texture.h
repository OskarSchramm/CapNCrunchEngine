//AUTHOR Oskar Schramm 2k22

#pragma once
#pragma comment( lib, "dxguid.lib")
#include <d3d11.h>
#include <DDSTextureLoader/DDSTextureLoader11.h>

struct ID3D11ShaderResourceView;

class Texture
{
public:
	Texture();
	~Texture();

	bool Initialize(ID3D11Device* aDevice, const char* aFilepath);
	void Shutdown();

	inline ID3D11ShaderResourceView* GetTexture() { return myTexture; }

private:
	ID3D11ShaderResourceView* myTexture;
};