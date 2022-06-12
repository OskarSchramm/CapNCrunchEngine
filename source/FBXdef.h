#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "CU/Vector3.hpp"
#include "CU/Vector2.hpp"

enum FBXTextureType
{
	cptnc_DIFFUSE,
	cptnc_NORMALS,
	cptnc_METALNESS,
	cptnc_ROUGHNESS,
	cptnc_AMBIENT_OCCLUSION,
	cptnc_DISPLACEMENT,
	cptnc_TEXTURETYPE_COUNT
};

struct Matrix
{
	float Data[16];

	bool operator==(const Matrix& other) const
	{
		return Data == other.Data;
	}
};

struct AABB
{
	float min[3];
	float max[3];
};

struct FBXVertex
{
	float position  [3] = {0, 0, 0};
	float normal    [3] = {0, 0, 0};
	float UVs       [2] = {0, 0};
	float tangents  [3] = {0, 0, 0};
	float bitangents[3] = {0 ,0, 0};
	float vertexColor[4][4] =
	{
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	};

	unsigned int boneIDs[4] = { 0, 0, 0, 0 };
	float boneWeights   [4] = { 0, 0, 0, 0 };
};

struct FBXSkeleton
{
	struct Bone
	{
		std::string name;
		Matrix bindPoseInverse;
		int parent;
		std::vector<unsigned int> children;

		bool operator==(const Bone& aBone) const
		{
			const bool A = bindPoseInverse == aBone.bindPoseInverse;
			const bool B = parent == aBone.parent;
			const bool C = name == aBone.name;
			const bool D = children == aBone.children;

			return A && B && C && D;
		}

		Bone() : bindPoseInverse{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, parent(-1)
		{}
	};

	std::string name;
	std::vector<Bone> bones;
	std::unordered_map<std::string, size_t> boneNameToIndex;

	const Bone* GetRoot() const { if (!bones.empty()) { return &bones[0]; } return nullptr; }

	bool operator==(const FBXSkeleton& aSkeleton) const
	{
		return bones == aSkeleton.bones;
	}
};

struct FBXModel
{
	struct FBXMesh
	{
		FBXMesh()
		{
			textureNames.resize(cptnc_TEXTURETYPE_COUNT);
			std::fill(textureNames.begin(), textureNames.end(), "Unassigned");
		}
		std::string name;

		std::vector<FBXVertex> vertices;
		std::vector<unsigned int> indices;

		unsigned int materialIndex;
		std::vector<std::string> textureNames = {"Unassigned"};

		AABB boundingBox;
	};

	const char* fbxName;
	std::vector<FBXMesh> meshes;
};

struct FBXModelAnimated
{
	struct FBXMesh
	{
		FBXMesh()
		{
			textureNames.resize(cptnc_TEXTURETYPE_COUNT);
			std::fill(textureNames.begin(), textureNames.end(), "Unassigned");
		}
		std::string name;

		std::vector<FBXVertex> vertices;
		std::vector<unsigned int> indices;

		unsigned int materialIndex;
		std::vector<std::string> textureNames = { "Unassigned" };

		AABB boundingBox;
	};

	const char* fbxName;
	std::vector<FBXMesh> meshes;
	FBXSkeleton skeleton;
};

struct FBXAnimation
{
	struct Frame
	{
		std::vector<Matrix> globalTransforms;
		std::vector<Matrix> localTransforms;
	};

	std::string name;
	std::vector<Frame> frames;
	unsigned int length;
	double duration;
	float framesPerSecond;
};