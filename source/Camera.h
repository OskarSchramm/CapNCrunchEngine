//AUTHOR Oskar Schramm 2k22

#pragma once
#include <DirectXMath.h>
#include "CU/Matrix4x4.hpp"
#include "CU/Vector2.hpp"

namespace CU
{
	class Input;
}

class Camera
{
public:
	Camera();
	~Camera();

	void Init(const CU::Vector3f& aPos, const CU::Vector3f& aRot, const float aFoV, const CU::Vector2f& aRes, const float aNearPlane, const float aFarPlane);
	void Update(const float aDT);

	void SetInput(CU::Input* aInput) { myInput = aInput; }

	void SetPosition(const CU::Vector3f& aPosition);
	void SetRotation(const CU::Vector3f& aRotation);

	void SetPerspectiveProjection(const float aHorizontalFoV, const CU::Vector2f& aResolution, const float aNearPlane, const float aFarPlane);

	inline CU::Matrix4x4f  GetViewMatrix() { return CU::Matrix4x4f::GetFastInverse(myTransform); }
	inline CU::Matrix4x4f& GetTransform()  { return myTransform; }
	inline CU::Matrix4x4f& GetProjectionMatrix() { return myProjectionMatrix; }
private:
	float myNearPlane;
	float myFarPlane;

	CU::Matrix4x4f myTransform;
	CU::Matrix4x4f myProjectionMatrix;
	float myMoveSpeed;
	float myRotationSpeed;

	CU::Input* myInput;
};