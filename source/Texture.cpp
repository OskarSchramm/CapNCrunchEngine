//AUTHOR Oskar Schramm 2k22

#include "Texture.h"
#include <string>

Texture::Texture()
	: myTexture(nullptr)
{}

Texture::~Texture(){}

bool Texture::Initialize(ID3D11Device* aDevice, const char* aFilename)
{
	HRESULT result;

	//Some string magic
	std::string ss(aFilename);
	std::wstring ws(ss.begin(), ss.end());

	result = DirectX::CreateDDSTextureFromFile(aDevice, ws.c_str(), nullptr, &myTexture);
	if (FAILED(result))
		return false;

	return true;
}

void Texture::Shutdown()
{
	if (myTexture)
	{
		myTexture->Release();
		myTexture = 0;
	}
}