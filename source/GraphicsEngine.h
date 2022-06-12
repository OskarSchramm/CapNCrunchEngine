//AUTHOR Oskar Schramm 2k22

#pragma once

//cpc
#include "D3D11Class.h"
#include "Camera.h"
#include "Light.h"
#include "ModelFactory.h"

//ShaderClasses
#include "ColorShader.h"
#include "TextureShader.h"
#include "LightShader.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;

class Model;

class GraphicsEngine
{
public:
	GraphicsEngine();
	~GraphicsEngine();

	void CreateModel(const char* aModelPath);

	bool Initialize(HWND, int aScreenWidth, int aScreenHeight);
	void Shutdown();

	void Update(const float aDT);
	bool Render();

	void SetInput(CU::Input* aInput) 
	{ 
		myInputptr = aInput;
		myCamera.SetInput(myInputptr);
	}

	inline D3D11Class* GetD3DClass() { return &myD3D; }
	inline ModelFactory* GetModelFactory() const { return myModelFactory; }
private:
	D3D11Class    myD3D;
	ModelFactory* myModelFactory;

	Model* 	    myModel;
	Camera      myCamera;
	Light 	    myLight;
	Texture     myCubeMap;

	CU::Input*  myInputptr;

	//ColorShader* myColorShader;
	//TextureShader* myTextureShader;
	LightShader myLightShader;
};
