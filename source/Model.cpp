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

void Model::Render(ID3D11DeviceContext* aDeviceContext)
{
	for (auto& t : myTextures)
		t.BindPS(aDeviceContext);

	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	aDeviceContext->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	aDeviceContext->IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	aDeviceContext->DrawIndexed(myIndexCount, 0, 0);
}