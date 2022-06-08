#include <iostream>
#include <fstream>

typedef struct
{
	float x, y, z;
}VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
}FaceType;

void GetModelFilename(char*);
bool ReadFileCounts(char*, int&, int&, int&, int&);
bool LoadDataStructures(char*, int, int, int, int);

int main()
{
	bool result;
	char filename[256];
	int vertexCount, textureCount, normalCount, faceCount;
	char garbage;

	GetModelFilename(filename);

	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
		return -1;

	std::cout << std::endl;
	std::cout << "Vertices: " << vertexCount << "\n";
	std::cout << "Normals: " << normalCount << "\n";
	std::cout << "UVs: " << textureCount << "\n";
	std::cout << "Faces: " << faceCount << "\n";

	result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
		return -1;

	std::cout << "\nFile has been converted" << "\n";
	std::cout << "\nDo you wish to exit (y/n)?";
	std::cin >> garbage;
}

void GetModelFilename(char* aFilename)
{
	bool done;
	std::ifstream fin;

	done = false;
	while (!done)
	{
		std::cout << "Enter model filename: ";
		std::cin >> aFilename;

		fin.open(aFilename);

		if (fin.good())
		{
			done = true;
		}
		else
		{
			fin.clear();
			std::cout << std::endl;
			std::cout << "File " << aFilename << " coult not be opened." << "\n\n";
		}
	}
}

bool ReadFileCounts(char* aFilename, int& aVertexCount, int& aTextureCount, int& aNormalCount, int& aFaceCount)
{
	std::ifstream fin;
	char input;

	aVertexCount = 0;
	aTextureCount = 0;
	aNormalCount = 0;
	aFaceCount = 0;

	fin.open(aFilename);
	if (fin.fail())
		return false;

	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { aVertexCount++; }
			if (input == 'n') { aNormalCount++; }
			if (input == 't') { aTextureCount++; }
		}

		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { aFaceCount++; }
		}

		while (input != '\n')
			fin.get(input);

		fin.get(input);
	}

	fin.close();

	return true;
}

bool LoadDataStructures(char* aFilename, int aVertexCount, int aTextureCount, int aNormalCount, int aFaceCount)
{
	VertexType* vertices = new VertexType[aVertexCount];
	VertexType* normals = new VertexType[aNormalCount];
	VertexType* texcoords = new VertexType[aTextureCount];
	FaceType* faces = new FaceType[aFaceCount];
	std::ifstream fin;
	int vertexIndex, normalIndex, texcoordIndex, faceIndex, vIndex, nIndex, tIndex;
	char input, input2;
	std::ofstream fout;

	vertexIndex = 0;
	normalIndex = 0;
	texcoordIndex = 0;
	faceIndex = 0;

	fin.open(aFilename);
	if (fin.fail())
		return false;

	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}

			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}
		}

		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		while (input != '\n')
			fin.get(input);

		fin.get(input);
	}

	//fin.close();

	fout.open("../data/models/txt/model.txt");

	fout << "Vertex Count: " << (aFaceCount * 3) << "\n";
	fout << "\n";
	fout << "Data:" << "\n";

	for (int i = 0; i < faceIndex; i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' ';
		fout << normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << ' ';
		fout << texcoords[tIndex].x << ' ' << texcoords[tIndex].y << '\n';

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << '\n';

		vIndex = faces[i].vIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << '\n';
	}

	fout.close();

	if (vertices)
	{
		delete[] vertices;
		vertices = nullptr;
	}

	if (normals)
	{
		delete[] normals;
		normals = nullptr;
	}

	if (texcoords)
	{
		delete[] texcoords;
		texcoords = nullptr;
	}

	if (faces)
	{
		delete[] faces;
		faces = nullptr;
	}

	return true;
}