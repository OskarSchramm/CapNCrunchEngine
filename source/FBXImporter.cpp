#include "FBXImporter.h"

#include "assimp/postprocess.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#include <assert.h>

FBXImporter::FBXImporter() = default;
FBXImporter::~FBXImporter() = default;

bool FBXImporter::LoadModel(const std::string& someFilePath, FBXModel& outModel)
{
	std::string filepath(someFilePath.begin(), someFilePath.end());

	auto aiScene = myImp.ReadFile(filepath.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices);

	InitFromScene(aiScene, someFilePath, outModel);

	return true;
}

void FBXImporter::InitFromScene(const aiScene* aScene, const std::string& someFilePath, FBXModel& outModel)
{
	const unsigned int meshCount = aScene->mNumMeshes;

	outModel.meshes.resize(meshCount);
	outModel.fbxName = aScene->mName.C_Str();

	if (aScene->HasMeshes())
	{
		for (size_t i = 0; i < meshCount; i++)
		{
			auto& aimesh = aScene->mMeshes[i];
			auto& outMesh = outModel.meshes[i];

			InitMesh(aimesh, outMesh);

			InitMaterial(aScene, aimesh, someFilePath, outMesh);
		}
	}
}

void FBXImporter::InitMesh(aiMesh*& aimesh, FBXModel::FBXMesh& outMesh)
{
	outMesh.name = aimesh->mName.C_Str();
	outMesh.materialIndex = aimesh->mMaterialIndex;

	outMesh.vertices.resize(aimesh->mNumVertices);
	outMesh.indices.resize(aimesh->mNumFaces * 3);

	//Vertices
	std::vector<FBXVertex> vertices;
	vertices.resize(aimesh->mNumVertices);
	for (size_t i = 0; i < outMesh.vertices.size(); i++)
	{
		const aiVector3D& pos    = aimesh->HasPositions()      ? aimesh->mVertices[i]         : aiVector3D(0, 0, 0);
		const aiVector3D& normal = aimesh->HasNormals()        ? aimesh->mNormals[i]          : aiVector3D(0, 0, 0);
		const aiVector3D& UV     = aimesh->HasTextureCoords(0) ? aimesh->mTextureCoords[0][i] : aiVector3D(0, 0, 0);

		vertices[i].position[0] = pos.x;
		vertices[i].position[1] = pos.y;
		vertices[i].position[2] = pos.z;

		vertices[i].normal[0] = normal.x;
		vertices[i].normal[1] = normal.y;
		vertices[i].normal[2] = normal.z;

		vertices[i].UVs[0] = UV.x;
		vertices[i].UVs[1] = UV.y;
	}
	//memcpy(&outMesh.vertices, &vertices, aimesh->mNumVertices * sizeof(FBXVertex));
	outMesh.vertices = vertices;

	//Indices
	std::vector<unsigned int> indices;
	unsigned int numIndices = aimesh->mNumFaces * 3;
	indices.resize(numIndices);
	int offset = 0;
	for (int i = 0; i < aimesh->mNumFaces; i++)
	{
		const aiFace& face = aimesh->mFaces[i];
		assert(face.mNumIndices == 3);
		int index = offset + i;
		indices[index]   = face.mIndices[0];
		indices[index+1] = face.mIndices[1];
		indices[index+2] = face.mIndices[2];
		offset += 2;
	}
	//memcpy(&outMesh.indices, &indices, sizeof(unsigned int) * numIndices);
	outMesh.indices = indices;

	memcpy(&outMesh.boundingBox, &aimesh->mAABB, sizeof(AABB));
}

void FBXImporter::InitMaterial(const aiScene* aScene, aiMesh*& aimesh, const std::string& someFilePath, FBXModel::FBXMesh& outMesh)
{
	if (AI_SCENE_FLAGS_INCOMPLETE)
		return;

	std::string::size_type slashIndex = someFilePath.find_last_of("/");
	std::string dir;

	if (slashIndex == std::string::npos)
		dir = ".";
	else if (slashIndex == 0)
		dir = "/";
	else
		dir = someFilePath.substr(0, slashIndex);

	//Try to get all the textures
	{
		outMesh.textureNames.resize(cptnc_TEXTURETYPE_COUNT);

		const aiMaterial* mat = aScene->mMaterials[outMesh.materialIndex];

		outMesh.textureNames[cptnc_DIFFUSE] = "Unassigned_Diffuse";
		TryGetTexture(mat, aiTextureType_DIFFUSE, outMesh.textureNames[cptnc_DIFFUSE]);

		outMesh.textureNames[cptnc_NORMALS] = "Unassigned_Normals";
		TryGetTexture(mat, aiTextureType_NORMALS, outMesh.textureNames[cptnc_NORMALS]);

		outMesh.textureNames[cptnc_METALNESS] = "Unassigned_Metalness";
		TryGetTexture(mat, aiTextureType_METALNESS, outMesh.textureNames[cptnc_METALNESS]);

		outMesh.textureNames[cptnc_ROUGHNESS] = "Unassigned_Roughness";
		TryGetTexture(mat, aiTextureType_DIFFUSE_ROUGHNESS, outMesh.textureNames[cptnc_ROUGHNESS]);

		outMesh.textureNames[cptnc_AMBIENT_OCCLUSION] = "Unassigned_Ambient_Occlusion";
		TryGetTexture(mat, aiTextureType_AMBIENT_OCCLUSION, outMesh.textureNames[cptnc_AMBIENT_OCCLUSION]);

		outMesh.textureNames[cptnc_DISPLACEMENT] = "Unassigned_Displacement";
		TryGetTexture(mat, aiTextureType_DISPLACEMENT, outMesh.textureNames[cptnc_DISPLACEMENT]);

		mat = nullptr;
	}
}

bool FBXImporter::TryGetTexture(const aiMaterial* aMat, aiTextureType aType, std::string& aOutPath)
{
	if (!aMat->GetTextureCount(aType) > 0)
		return false;

	aiString path;
	if (!aMat->GetTexture(aType, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		return false;

	aOutPath = (path.data);
	return true;
}