//AUTHOR Oskar Schramm 2k22
#pragma once
#include "Camera.h"
#include "CU/Input.h"

#define KeyCode unsigned char
#define KeyDown(KeyCode) myInput->GetKeyboard()->KeyIsPressed(KeyCode)

Camera::Camera() : myMoveSpeed(0.1f), myRotationSpeed(0.05f) {}
Camera::~Camera() {}

void Camera::Init(const CU::Vector3f& aPos, const CU::Vector3f& aRot, const float aFoV, const CU::Vector2i& aRes, const float aNearPlane, const float aFarPlane)
{
	myViewMatrix.SetPositionRelative(aPos);
	myViewMatrix.SetRotation(aRot);
	SetPerspectiveProjection(aFoV, aRes, aNearPlane, aFarPlane);
}

void Camera::SetPerspectiveProjection(const float aHorizontalFoV, const CU::Vector2i& aResolution, const float aNearPlane, const float aFarPlane)
{
	myProjectionMatrix = {};

	myNearPlane = aNearPlane;
	myFarPlane = aFarPlane;

	const float hFovRad = aHorizontalFoV * (3.1415f / 180.0f);

	const float vFovRad = 2 * std::atan(std::tan(hFovRad / 2.0f) * ((float)aResolution.y / (float)aResolution.x));

	float xScale = 1.0f / std::tanf(hFovRad / 2.0f);
	float yScale = 1.0f / std::tanf(vFovRad / 2.0f);
	float q = myFarPlane / (myFarPlane - myNearPlane);

	myProjectionMatrix(1, 1) = xScale;
	myProjectionMatrix(2, 2) = yScale;
	myProjectionMatrix(3, 3) = q;
	myProjectionMatrix(3, 4) = 1.0f / q;
	myProjectionMatrix(4, 3) = -q * myNearPlane;
	myProjectionMatrix(4, 4) = 0.0f;
}

void Camera::Update(const float aDT)
{
	CU::Vector3f nextPos = {};
	CU::Vector3f nextRot = {};

	if (KeyDown('W'))
	{
		nextPos.z += myMoveSpeed * aDT;
	}
	if (KeyDown('A'))
	{
		nextPos.x -= myMoveSpeed * aDT;
	}
	if (KeyDown('S'))
	{
		nextPos.z -= myMoveSpeed * aDT;
	}
	if (KeyDown('D'))
	{
		nextPos.x += myMoveSpeed * aDT;
	}

	if (KeyDown('Q'))
	{
		nextRot.y -= myRotationSpeed * aDT;
	}
	if (KeyDown('E'))
	{
		nextRot.y += myRotationSpeed * aDT;
	}

	if (KeyDown('R'))
	{
		nextRot.x -= myRotationSpeed * aDT;
	}
	if (KeyDown('T'))
	{
		nextRot.x += myRotationSpeed * aDT;
	}

	SetPosition(nextPos);
	SetRotation(nextRot);
}

void Camera::SetPosition(const CU::Vector3f& aPosition)
{
	myViewMatrix = CU::Matrix4x4f::CreateTranslationMatrix(aPosition) * myViewMatrix;
}

void Camera::SetRotation(const CU::Vector3f& aRotation)
{
	myViewMatrix = CU::Matrix4x4f::CreateRotationAroundX(aRotation.x) * myViewMatrix;
	myViewMatrix = CU::Matrix4x4f::CreateRotationAroundY(aRotation.y) * myViewMatrix;
	myViewMatrix = CU::Matrix4x4f::CreateRotationAroundZ(aRotation.z) * myViewMatrix;
}