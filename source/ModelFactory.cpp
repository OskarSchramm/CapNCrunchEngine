//AUTHOR Oskar Schramm 2k22

#include "ModelFactory.h"
#include "FBXImporter.h"
#include <fstream>
#include <array>

#include "Model.h"

#define ModelExists(filepath) myModels.find(filepath) != myModels.end()

namespace
{
	typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;
#pragma warning(disable:4146)
	uint32_t pcg32_random_r(pcg32_random_t* rng)
	{
		uint64_t oldstate = rng->state;
		// Advance internal state
		rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
		// Calculate output function (XSH RR), uses old state for max ILP
		uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
		uint32_t rot = oldstate >> 59u;
		return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}

	// Returns a random float between 0.0 and 1.0
	float RandomFloat()
	{
		thread_local pcg32_random_t state;
		return (float)ldexp(pcg32_random_r(&state), -32);
	}

	void AddNoise(std::vector<float>& buffer, float amount)
	{
		for (int i = 0; i < buffer.size(); i++)
		{
			buffer[i] += (2.0f * RandomFloat() - 1.0f) * amount;
		}
	}

	std::vector<float> Upsample2X(const std::vector<float>& input, int resolution)
	{
		std::vector<float> output(2 * resolution * 2 * resolution);


		// corners
		output[0] = input[0];
		output[2 * resolution * (2 * resolution - 1)] = input[resolution * (resolution - 1)];
		output[2 * resolution - 1] = input[resolution - 1];
		output[2 * resolution * (2 * resolution - 1) + (2 * resolution - 1)] = input[resolution * (resolution - 1) + (resolution - 1)];

		for (int i = 0; i < resolution - 1; i++)
		{
			// edge where j = 0
			{
				float in0 = input[resolution * (i + 0)];
				float in1 = input[resolution * (i + 1)];

				output[2 * resolution * (2 * i + 1)] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[2 * resolution * (2 * i + 2)] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}

			// interior
			for (int j = 0; j < resolution - 1; j++)
			{
				float in00 = input[resolution * (i + 0) + j + 0];
				float in01 = input[resolution * (i + 1) + j + 0];
				float in10 = input[resolution * (i + 0) + j + 1];
				float in11 = input[resolution * (i + 1) + j + 1];

				output[2 * resolution * (2 * i + 1) + 2 * j + 1] = 0.25f * 0.25f * (3.0f * 3.0f * in00 + 3.0f * 1.0f * in01 + 1.0f * 3.0f * in10 + 1.0f * 1.0f * in11);
				output[2 * resolution * (2 * i + 2) + 2 * j + 1] = 0.25f * 0.25f * (3.0f * 1.0f * in00 + 3.0f * 3.0f * in01 + 1.0f * 1.0f * in10 + 1.0f * 3.0f * in11);
				output[2 * resolution * (2 * i + 1) + 2 * j + 2] = 0.25f * 0.25f * (1.0f * 3.0f * in00 + 1.0f * 1.0f * in01 + 3.0f * 3.0f * in10 + 3.0f * 1.0f * in11);
				output[2 * resolution * (2 * i + 2) + 2 * j + 2] = 0.25f * 0.25f * (1.0f * 1.0f * in00 + 1.0f * 3.0f * in01 + 3.0f * 1.0f * in10 + 3.0f * 3.0f * in11);
			}

			// edge where j = is resolution-1
			{
				float in0 = input[resolution * (i + 0) + resolution - 1];
				float in1 = input[resolution * (i + 1) + resolution - 1];

				output[2 * resolution * (2 * i + 1) + 2 * resolution - 1] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[2 * resolution * (2 * i + 2) + 2 * resolution - 1] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}
		}

		for (int j = 0; j < resolution - 1; j++)
		{
			// edge where i = 0
			{
				float in0 = input[j + 0];
				float in1 = input[j + 1];

				output[(2 * j + 1)] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[(2 * j + 2)] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}

			// edge where i = is resolution-1
			{
				float in0 = input[resolution * (resolution - 1) + j + 0];
				float in1 = input[resolution * (resolution - 1) + j + 1];

				output[2 * resolution * (2 * resolution - 1) + (2 * j + 1)] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[2 * resolution * (2 * resolution - 1) + (2 * j + 2)] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}
		}

		return output;
	}
}

ModelFactory::ModelFactory(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext)
	: myDevice(aDevice), myDeviceContext(aDeviceContext)
{}

ModelFactory::~ModelFactory()
{}

Model* ModelFactory::LoadModel(const std::wstring& someFilePath)
{
	std::string filepath(someFilePath.begin(), someFilePath.end());

	if (ModelExists(filepath))
		return myModels.at(filepath);

	FBXImporter fbxI;
	
	FBXModel* fbxM = new FBXModel();
	fbxI.LoadModel(filepath, *fbxM);

	Model* model = new Model();

	if (!InitializeBuffers(fbxM, model))
		//return;

	//if (!InitToAppropriateShader())
		//return;

	filepath = "bin/data/textures/cool_alien_bricks.dds";
	if(!AssignTextures(model, filepath))

	myModels.insert(std::make_pair(filepath, model));
	return model;
}

bool ModelFactory::InitializeBuffers(FBXModel* anInModel, Model* aOutModel)
{
	//Convert to my model vertices
	unsigned int vertexCount = anInModel->meshes[0].vertices.size();
	unsigned int indexCount  = anInModel->meshes[0].indices.size();

	Vertex* vertices = new Vertex[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];

	aOutModel->myIndexCount = indexCount;

	auto& baseMesh = anInModel->meshes[0];
	for (int i = 0; i < vertexCount; i++)
	{
		auto& vert = baseMesh.vertices[i];

		auto& pos = vert.position;
		auto& nor = vert.normal;
		auto& UVs = vert.UVs;

		vertices[i].position = { pos[0], pos[1], pos[2]};
		vertices[i].UVs = { UVs[0], UVs[1]};
		vertices[i].normal = { nor[0], nor[1], nor[2]};

		indices[i] = i;
	}

	HRESULT result;

	//Create vertex and index buffers
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;

	//VertexBuffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * anInModel->meshes[0].vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = myDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &aOutModel->myVertexBuffer);
	if (FAILED(result))
		return false;

	//IndexBuffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = myDevice->CreateBuffer(&indexBufferDesc, &indexData, &aOutModel->myIndexBuffer);
	if (FAILED(result))
		return false;

	return true;
}

Model* ModelFactory::LoadModelObj(const char* someFileName)
{
	HRESULT result;

	std::string filepath(someFileName);

	if (myModels.find(filepath) != myModels.end())
		return myModels.at(filepath);

	Model* model = new Model();
	std::ifstream fin;
	char input;
	int i;

	fin.open(filepath);
	if (fin.fail())
		return nullptr;

	fin.get(input);
	while (input != ':')
		fin.get(input);

	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;

	fin >> vertexCount;
	indexCount = vertexCount;

	fin.get(input);
	while (input != ':')
		fin.get(input);

	fin.get(input);
	fin.get(input);

	model->myVertexCount = vertexCount;
	model->myIndexCount  = indexCount;

	Vertex* vertices;
	unsigned long* indices;
	vertices = new Vertex[vertexCount];
	indices = new unsigned long[indexCount];

	for (i = 0; i < model->myVertexCount; i++)
	{
		fin >> vertices[i].position.x >> vertices[i].position.y >> vertices[i].position.z;
		fin >> vertices[i].normal.x   >> vertices[i].normal.y   >> vertices[i].normal.z;
		fin >> vertices[i].UVs.x      >> vertices[i].UVs.y;

		indices[i] = i;
	}

	fin.close();

	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;

	vertexData.pSysMem = vertices;
	indexData.pSysMem = indices;

	D3D11_BUFFER_DESC vertexDesc = {};
	vertexDesc.ByteWidth = vertexCount * sizeof(Vertex);
	vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	result = myDevice->CreateBuffer(&vertexDesc, &vertexData, &model->myVertexBuffer);
	if (FAILED(result))
		return nullptr;

	D3D11_BUFFER_DESC indexDesc = {};
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexDesc.ByteWidth = indexCount * sizeof(unsigned long);
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	result = myDevice->CreateBuffer(&indexDesc, &indexData, &model->myIndexBuffer);
	if (FAILED(result))
		return nullptr;

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	AssignTextures(model, "bin/data/textures/cool_alien_bricks.dds");

	myModels.insert(std::make_pair(filepath, model));
	return model;
}

bool ModelFactory::InitializeBuffers(Model* aOutModel, Vertex* vertices, const unsigned vertexCount, unsigned int* indices, const unsigned int indexCount)
{
	HRESULT result;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;

	vertexData.pSysMem = vertices;
	indexData.pSysMem = indices;

	D3D11_BUFFER_DESC vertexDesc = {};
	vertexDesc.ByteWidth = vertexCount * sizeof(Vertex);
	vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	result = myDevice->CreateBuffer(&vertexDesc, &vertexData, &aOutModel->myVertexBuffer);
	if (FAILED(result))
		return false;

	D3D11_BUFFER_DESC indexDesc = {};
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexDesc.ByteWidth = indexCount * sizeof(unsigned long);
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	result = myDevice->CreateBuffer(&indexDesc, &indexData, &aOutModel->myIndexBuffer);
	if (FAILED(result))
		return false;

	aOutModel->myVertexCount = vertexCount;
	aOutModel->myIndexCount  = indexCount;

	return true;
}

bool ModelFactory::AssignTextures(Model* aModel, const std::string& aFilepath)
{
	//std::wstring filepath = std::wstring(aFilepath.begin(), aFilepath.end());

	if (!aModel->myTexture.Initialize(myDevice, aFilepath.c_str()))
		return false;

	return true;
}

Model* ModelFactory::GenerateTerrain()
{
	std::vector<float> heightMap;

	float resolution = 16.0f;
	float amplitude = 1.2f;

	heightMap.resize(resolution * resolution);
	for (size_t i = 0; i < 4; i++)
	{
		AddNoise(heightMap, amplitude);
		amplitude *= 0.25f;

		heightMap = Upsample2X(heightMap, resolution);
		resolution *= 2.0f;
	}

	Vertex* vertices = new Vertex[heightMap.size()];
	GenerateVertices(vertices, heightMap, resolution);
	GenerateNormals(vertices, heightMap.size(), resolution);

	unsigned int indexCount = ((resolution - 1) * (resolution - 1)) * 6;
	unsigned int* indices = new unsigned int[indexCount];
	GenerateIndices(indices, indexCount, resolution);

	Model* aModel = new Model();
	if (!InitializeBuffers(aModel, vertices, heightMap.size(), indices, indexCount))
		return nullptr;

	const std::string texturePath = "bin/data/textures/cool_alien_bricks.dds";
	const std::string randomModelName = "terrain01";
	if (!AssignTextures(aModel, texturePath));

	myModels.insert(std::make_pair(randomModelName, aModel));

	delete[] vertices;
	delete[] indices;
	vertices = nullptr;
	indices = nullptr;

	return aModel;
}

void ModelFactory::GenerateVertices(Vertex* outVertices, const std::vector<float>& aHeightMap, const float aResolution)
{
	float distanceScalar = 0.04f;
	float uvScalar = 20.0f;

	for (size_t i = 0; i < aHeightMap.size(); i++)
	{
		size_t row = i / (int)aResolution;
		size_t column = i % (int)aResolution;

		//Position
		outVertices[i].position.x = (float)row * distanceScalar;
		outVertices[i].position.y = aHeightMap[i];
		outVertices[i].position.z = (float)column * distanceScalar;

		//UV
		outVertices[i].UVs.x = row * (1 / aResolution) * uvScalar;
		outVertices[i].UVs.y = column * (1 / aResolution) * uvScalar;
	}
}

void ModelFactory::GenerateNormals(Vertex* outVertices, const size_t amountVertices, const float aResolution)
{
	for (size_t i = 0; i < amountVertices; i++)
	{
		size_t row = i / (int)aResolution;
		size_t column = i % (int)aResolution;

		if (row == aResolution - 1 || column == aResolution - 1)
			continue;
		if (row == 0 || column == 0)
			continue;

		CU::Vector3f above = outVertices[i - (int)aResolution].position;
		CU::Vector3f below = outVertices[i + (int)aResolution].position;
		CU::Vector3f right = outVertices[i + 1].position;
		CU::Vector3f left = outVertices[i - 1].position;

		CU::Vector3f vertical = below - above;
		CU::Vector3f horizontal = right - left;

		CU::Vector3f Ncross = (horizontal.Cross(vertical)).GetNormalized();

		outVertices[i].normal.x = Ncross.x;
		outVertices[i].normal.y = Ncross.y;
		outVertices[i].normal.z = Ncross.z;
	}
}

void ModelFactory::GenerateIndices(unsigned int* outIndices, const size_t indexCount, const float aResolution)
{
	int counter = 0;
	for (size_t i = 0; i < indexCount / 6; i++)
	{
		size_t row = i / (int)aResolution;
		size_t column = i % (int)aResolution;

		if (row == aResolution - 1 || column == aResolution - 1)
			continue;

		size_t bottomLeft = (row + 1) * aResolution + column;
		size_t bottomRight = (row + 1) * aResolution + (column + 1);
		size_t topLeft = row * aResolution + column;
		size_t topRight = row * aResolution + (column + 1);

		outIndices[counter] = bottomLeft;
		counter++;
		outIndices[counter] = topLeft;
		counter++;
		outIndices[counter] = topRight;
		counter++;
		outIndices[counter] = topRight;
		counter++;
		outIndices[counter] = bottomRight;
		counter++;
		outIndices[counter] = bottomLeft;
		counter++;
	}
}