//AUTHOR Oskar Schramm 2k22

#include "Model.h"
#include <d3d11.h>
#include <DirectXMath.h>

#include <fstream>
#include "Vertex.h"

Model::Model()
	: myVertexBuffer(nullptr), myIndexBuffer(nullptr), myIndexCount(0), myVertexCount(0)
{}
Model::~Model() {}

void Model::Shutdown()
{
	ReleaseTexture();

	ShutdownBuffers();
}

bool Model::Init(ID3D11Device* aDevice)
{
	//IDK

	return true;
}

void Model::ShutdownBuffers()
{
	if (myIndexBuffer)
	{
		myIndexBuffer->Release();
		myIndexBuffer = nullptr;
	}

	if (myVertexBuffer)
	{
		myVertexBuffer->Release();
		myVertexBuffer = nullptr;
	}
}

void Model::ReleaseTexture()
{
	if (myTexture.GetTexture())
		myTexture.Shutdown();
}

void Model::Render(ID3D11DeviceContext* aDeviceContext)
{
	auto texture = myTexture.GetTexture();
	if (texture)
		aDeviceContext->PSSetShaderResources(0, 1, &texture);

	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	aDeviceContext->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	aDeviceContext->IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	aDeviceContext->DrawIndexed(myIndexCount, 0, 0);
}