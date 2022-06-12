#pragma once
#include "CU/Vector2.hpp"
#include "CU/Vector3.hpp"
#include "CU/Vector4.hpp"

struct Vertex
{
	CU::Vector3f position;
	CU::Vector2<float> uv;
	CU::Vector3<float> normal;
	CU::Vector3<float> tangent;
	CU::Vector3<float> bitangent;

	Vertex() {}
	Vertex(float X, float Y, float Z, float nX, float nY, float nZ, float tU, float tV)
	{
		position = { X, Y, Z};
		uv      = { tU, tV };
		normal   = { nX, nY, nZ };
	}
};