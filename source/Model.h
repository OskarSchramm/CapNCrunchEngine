//AUTHOR Oskar Schramm 2k22

#pragma once
#include "TextureTypes.h"
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

	void Render(ID3D11DeviceContext*);
	inline CU::Matrix4x4f& GetModelMatrix() { return myTransform; }
private:
	ID3D11Buffer* myVertexBuffer;
	ID3D11Buffer* myIndexBuffer;
	int myVertexCount;
	int myIndexCount;

	std::vector<Texture> myTextures;

	CU::Matrix4x4f myTransform;
};