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

	myCamera.Init(CU::Vector3f{ 0.0f, 5.0f, -5.0f }, { 45.0f, 45.0f, 0.0f }, 90.0f, CU::Vector2f{ (float)aScreenWidth, (float)aScreenHeight }, 0.01f, 1000.0f);

	myLight.SetDirectionalLight({ 0.5f, -0.5f, 0.0f }, { 0.1f, 0.1f, 0.1, 0.5f });
	myLight.SetAmbientLight({ 0.9f, 0.35f, 0.25f, 1.0f }, { 0.65f, 0.5f, 0.37f, 1.0f });
	myCubeMap.Init(myD3D.GetDevice(), myD3D.GetDeviceContext(), L"bin/data/textures/dds/cube_skansen.dds", false, true, true);
	myCubeMap.SetSlotIndex(TextureType::CubeMap);

	myModel = myModelFactory->GenerateTerrain();

	return true;
}

void GraphicsEngine::Update(float aDT)
{
	myCamera.Update(aDT);

	float rotSpeed = 3.0f;
	myLight.SetDirectionalLight({ std::sin(aDT * rotSpeed), std::cos(aDT * rotSpeed), 0.0f }, { 1.0f, 1.0f, 1.0, 1.0f });
}

bool GraphicsEngine::Render()
{
	if (!myModel)
		return false;

	if(!myLightShader.Render(myD3D.GetDeviceContext(), &myLight, &myCamera, myModel))
		return false;

	myCubeMap.BindPS(myD3D.GetDeviceContext());

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