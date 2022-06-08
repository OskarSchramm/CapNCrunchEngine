//AUTHOR Oskar Schramm 2k22

#pragma once
#include "Texture.h"
#include "Vertex.h"
#include <vector>
#include "CU/Matrix4x4.hpp"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;

class Model
{
	friend class ModelFactory;

public:
	Model();
	~Model();

	bool Init(ID3D11Device* aDevice);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	inline int GetIndexCount() const { return myIndexCount; }

	inline CU::Matrix4x4f& GetModelMatrix() { return myTransform; }

	inline ID3D11ShaderResourceView* GetTexture() { return myTexture.GetTexture(); }
private:
	void ShutdownBuffers();
	void ReleaseTexture();

	ID3D11Buffer* myVertexBuffer;
	ID3D11Buffer* myIndexBuffer;
	int myVertexCount;
	int myIndexCount;

	Texture myTexture;

	CU::Matrix4x4f myTransform;
};