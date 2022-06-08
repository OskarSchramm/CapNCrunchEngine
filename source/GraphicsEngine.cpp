//AUTHOR Oskar Schramm 2k22

#include "GraphicsEngine.h"

GraphicsEngine::GraphicsEngine()
	: myModelFactory(nullptr)
{}

GraphicsEngine::~GraphicsEngine() {}

bool GraphicsEngine::Initialize(HWND aHWND, int aScreenWidth, int aScreenHeight)
{
	bool result;

	result = myD3D.Initialize(aScreenWidth, aScreenHeight, VSYNC_ENABLED, aHWND, FULL_SCREEN);
	if (!result)
	{
		MessageBox(aHWND, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	result = myLightShader.Init(myD3D.GetDevice(), &aHWND);
	if(!result)
	{
		MessageBox(aHWND, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	myModelFactory = new ModelFactory(myD3D.GetDevice(), myD3D.GetDeviceContext());

	myCamera.Init(CU::Vector3f{ 0.0f, 0.0f, -10.0f }, CU::Vector3f::Zero, 90.0f, CU::Vector2i{ aScreenWidth, aScreenHeight }, 0.01f, 1000.0f);

	myLight.SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	myLight.SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	myLight.SetDirection(1.0f, 0.0f, 1.0f);
	myLight.SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	myLight.SetSpecularPower(32.0f);

	myModel = myModelFactory->GenerateTerrain();

	return true;
}

void GraphicsEngine::Update(float aDT)
{
	myCamera.Update(1.0f);

	static float rotation;
	rotation += (float)CU::Pi * 0.005f;
	if (rotation > 360.0f)
		rotation -= 360.0f;

	myModel->GetModelMatrix().CreateRotationAroundX(rotation);
}

bool GraphicsEngine::Render()
{
	if (!myModel)
		return false;

	bool result;

	result = myLightShader.Render(myD3D.GetDeviceContext(), &myLight, &myCamera, myModel);
	if (!result)
		return false;

	return true;
}

void GraphicsEngine::CreateModel(const char* aModelPath)
{
	const char* tempCube = "bin/data/models/txt/model.txt";
	auto model = myModelFactory->LoadModelObj(tempCube);

	myModel = model;
	myModel->GetModelMatrix().SetPositionRelative({ 0.0f, 0.0f, 0.0f });
}

void GraphicsEngine::Shutdown()
{

}