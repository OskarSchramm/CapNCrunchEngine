//AUTHOR Oskar Schramm 2k22

#pragma once
#include "CU/Vector3.hpp"
#include "CU/Vector4.hpp"

class Light
{
public:
	Light();
	~Light();

	inline void SetAmbientColor(const float aRed, const float aBlue, const float aGreen, const float aAlpha)
	{
		myAmbientColor = CU::Vector4f(aRed, aBlue, aGreen, aAlpha);
	}
	inline void SetDiffuseColor(const float aRed, const float aBlue, const float aGreen, const float aAlpha) 
	{
		myDiffuseColor = CU::Vector4f(aRed, aBlue, aGreen, aAlpha);
	}
	inline void SetDirection(const float aX, const float aY, const float aZ)
	{
		myDirection = CU::Vector3f(aX, aY, aZ);
	}
	inline void SetSpecularColor(const float aRed, const float aBlue, const float aGreen, const float aAlpha)
	{
		mySpecularColor = CU::Vector4f(aRed, aBlue, aGreen, aAlpha);
	}
	inline void SetSpecularPower(const float aPower)
	{
		mySpecularPower = aPower;
	}

	inline CU::Vector4f GetAmbientColor()  const { return myAmbientColor; }
	inline CU::Vector4f GetDiffuseColor()  const { return myDiffuseColor; }
	inline CU::Vector3f GetDirection()     const { return myDirection; }
	inline CU::Vector4f GetSpecularColor() const { return mySpecularColor; }
	inline float        GetSpecularPower() const { return mySpecularPower; }
private:
	CU::Vector4f myAmbientColor;
	CU::Vector4f myDiffuseColor;
	CU::Vector3f myDirection;
	CU::Vector4f mySpecularColor;
	float mySpecularPower;
};