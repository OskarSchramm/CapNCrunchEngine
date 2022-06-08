#pragma once
#include <string>
#include "assimp/material.h"
#include "assimp/Importer.hpp"
#include "FBXdef.h"

struct aiScene;
struct aiMesh;
struct aiMaterial;

class FBXImporter
{
public:
	FBXImporter();
	~FBXImporter();
	FBXImporter(const FBXImporter&) = delete;

	bool LoadModel(const std::string& someFilePath, FBXModel& outModel);
	//bool IsValidModelFile(const std::string& someFilePath);
	//bool LoadAnimation(const std::string& someFilePath, const std::vector<std::string>& boneNames, FBXAnimation& outAnimation);
	//bool LoadSkeleton(const std::string& someFilePath, FBXModel& outModel);
private:
	void InitFromScene(const aiScene* aScene, const std::string& someFilePath, FBXModel& outModel);
	void InitMesh(aiMesh*& aimesh, FBXModel::FBXMesh& outModel);
	void InitMaterial(const aiScene* aScene, aiMesh*& aimesh, const std::string& someFilePath, FBXModel::FBXMesh& outMesh);
	bool TryGetTexture(const aiMaterial* aMat, aiTextureType aType, std::string& aOutPath);

	Assimp::Importer myImp;
};