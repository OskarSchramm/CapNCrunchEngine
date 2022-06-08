#pragma once
#include "CU/Vector2.hpp"
#include "CU/Vector3.hpp"
#include "CU/Vector4.hpp"

struct Vertex
{
	CU::Vector3f position;
	CU::Vector3<float> normal;
	CU::Vector2<float> UVs;

	Vertex() {}
	Vertex(float X, float Y, float Z, float nX, float nY, float nZ, float tU, float tV)
	{
		position = { X, Y, Z};
		UVs      = { tU, tV };
		normal   = { nX, nY, nZ };
	}
};