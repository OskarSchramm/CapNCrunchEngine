//AUTHOR Oskar Schramm 2k22

#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "FBXdef.h"

class Model;
struct Vertex;
struct ID3D11Device;
struct ID3D11DeviceContext;

class ModelFactory
{
public:
	ModelFactory(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext);
	~ModelFactory();

	Model* LoadModel(const std::wstring& someFilePath);
	Model* LoadModelObj(const char* aFilename); //Legacy

	Model* GenerateTerrain();
private:
	void GenerateVertices(Vertex* outVertices, const std::vector<float>& aHeightMap, const float aResolution);
	void GenerateNormals(Vertex* outVertices, const size_t amountVertices, const float aResolution);
	void GenerateIndices(unsigned int* outIndices, const size_t indexCount, const float aResolution);

	bool InitializeBuffers(FBXModel* anInModel, Model* aOutModel);
	bool InitializeBuffers(Model* aOutModel, Vertex* vertices, const unsigned vertexCount, unsigned int* indices, const unsigned int indexCount);

	bool AssignTextures(Model* aModel, const std::string* texturePaths, size_t textureAmount, bool useSRGB, bool generateMipMaps);

	ID3D11Device* myDevice;
	ID3D11DeviceContext* myDeviceContext;

	std::unordered_map<std::string, Model*> myModels;
};