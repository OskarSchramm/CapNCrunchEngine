//AUTHOR Oskar Schramm 2k22

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include "LightShader.h"
#include <d3d11.h>
#include <fstream>

#include "CU/Matrix4x4.hpp"

LightShader::LightShader()
	: myVertexShader(nullptr), myPixelShader(nullptr), myLayout(nullptr), mySampleState(nullptr),
	myFrameBuffer(nullptr), myObjectBuffer(nullptr), myCameraBuffer(nullptr), myLightBuffer(nullptr)
{}

LightShader::~LightShader()
{}

bool LightShader::Init(ID3D11Device* aDevice, HWND* aHWND)
{
	bool result;

	if (!InitShaders(aDevice, aHWND, (WCHAR*)L"bin/shaders/LightVS", (WCHAR*)L"bin/shaders/LightPS"))
		return false;

	if (!CreateConstantBuffers(aDevice))
		return false;

	if (!CreateSamplerState(aDevice))
		return false;

	return true;
}

bool LightShader::InitShaders(ID3D11Device* aDevice, HWND* aHWND, WCHAR* aVSFilename, WCHAR* aPSFilename)
{
	HRESULT result;
	ID3DBlob* errorMSG = nullptr;
	ID3DBlob* vertexShaderBlob = nullptr;
	ID3DBlob* pixelShaderBlob = nullptr;

	if (!ReadShaderFile(aHWND, aVSFilename, &vertexShaderBlob, &errorMSG))
		return false;

	if (!ReadShaderFile(aHWND, aPSFilename, &pixelShaderBlob, &errorMSG))
		return false;

	result = aDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &myVertexShader);
	if (FAILED(result))
		return false;

	result = aDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &myPixelShader);
	if (FAILED(result))
		return false;

	if (!SetPolygonLayout(aDevice, vertexShaderBlob))
		return false;

	//Cleanup
	vertexShaderBlob->Release();
	vertexShaderBlob = nullptr;
	pixelShaderBlob->Release();
	pixelShaderBlob = nullptr;

	return true;
}

bool LightShader::CreateConstantBuffers(ID3D11Device* aDevice)
{
	HRESULT result;

	//Buffer Setup
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//Object Buffer
	{
		bufferDesc.ByteWidth = sizeof(ObjectBuffer);
		result = aDevice->CreateBuffer(&bufferDesc, NULL, &myObjectBuffer);
		if (FAILED(result))
			return false;
	}

	//Frame Buffer
	{
		bufferDesc.ByteWidth = sizeof(FrameBuffer);
		result = aDevice->CreateBuffer(&bufferDesc, NULL, &myFrameBuffer);
		if (FAILED(result))
			return false;
	}

	//LightBuffer
	{
		bufferDesc.ByteWidth = sizeof(LightBuffer);
		result = aDevice->CreateBuffer(&bufferDesc, NULL, &myLightBuffer);
		if (FAILED(result))
			return false;
	}

	//Camera buffer
	{
		bufferDesc.ByteWidth = sizeof(CameraBuffer);
		result = aDevice->CreateBuffer(&bufferDesc, NULL, &myCameraBuffer);
		if (FAILED(result))
			return false;
	}

	return true;
}

bool LightShader::CreateSamplerState(ID3D11Device* aDevice)
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT result = aDevice->CreateSamplerState(&samplerDesc, &mySampleState);
	if (FAILED(result))
		return false;

	return true;
}

bool LightShader::ReadShaderFile(HWND* anHWND, WCHAR* aShaderFilename, ID3DBlob** aShaderBlob, ID3DBlob** anErrorBlob)
{
	HRESULT result;

	std::wstring shaderFilenameCso(aShaderFilename);
	std::wstring shaderFilenameHlsl(aShaderFilename);
	shaderFilenameCso = shaderFilenameCso + L".cso";
	shaderFilenameHlsl = shaderFilenameHlsl + L".hlsl";

	result = D3DReadFileToBlob(shaderFilenameCso.c_str(), aShaderBlob);
	if (FAILED(result))
	{
		result = D3DCompileFromFile(shaderFilenameHlsl.c_str(), NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, aShaderBlob, anErrorBlob);
		if (FAILED(result))
		{
			if (anErrorBlob)
				OutputShaderErrorMessage(*anErrorBlob, *anHWND, aShaderFilename);
			else
				MessageBox(*anHWND, aShaderFilename, L"Missing Shader File", MB_OK);

			return false;
		}
	}
}

bool LightShader::SetPolygonLayout(ID3D11Device* const aDevice, ID3DBlob* aVertexShaderBlob)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	HRESULT result = aDevice->CreateInputLayout(polygonLayout, numElements, aVertexShaderBlob->GetBufferPointer(), aVertexShaderBlob->GetBufferSize(), &myLayout);
	if (FAILED(result))
		return false;

	return true;
}

void LightShader::OutputShaderErrorMessage(ID3DBlob* anErrorMSG, HWND aHWND, WCHAR* aShaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize;
	ofstream fout;

	compileErrors = (char*)(anErrorMSG->GetBufferPointer());

	bufferSize = anErrorMSG->GetBufferSize();

	fout.open("shader-error.txt");

	for (unsigned long i = 0; i < bufferSize; i++)
		fout << compileErrors[i];

	fout.close();

	anErrorMSG->Release();
	anErrorMSG = nullptr;

	MessageBox(aHWND, L"Error compiling shader. Check shader-error.txt for message", aShaderFilename, MB_OK);

	return;
}

bool LightShader::SetObjectData(ID3D11DeviceContext* aDeviceContext, Model* aModel)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	{
		//ObjectBuffer
		ObjectBuffer objectBufferData = {};
		objectBufferData.modelMatrix = aModel->GetModelMatrix();

		aDeviceContext->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &objectBufferData, sizeof(ObjectBuffer));
		aDeviceContext->Unmap(myObjectBuffer, 0);

		aDeviceContext->VSSetConstantBuffers(0, 1, &myObjectBuffer);
	}

	return true;
}

bool LightShader::UpdateAndBindBuffers(ID3D11DeviceContext* aDeviceContext, Light* aLight, Camera* aCamera)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	{
		ZeroMemory(&mappedResource, sizeof(mappedResource));

		//FrameBuffer
		FrameBuffer frameBufferData = {};
		auto invCam = CU::Matrix4x4f::GetFastInverse(aCamera->GetViewMatrix()) * aCamera->GetProjectionMatrix();
		frameBufferData.worldToClipMatrix = invCam;

		aDeviceContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &frameBufferData, sizeof(FrameBuffer));
		aDeviceContext->Unmap(myFrameBuffer, 0);

		aDeviceContext->VSSetConstantBuffers(1, 1, &myFrameBuffer);
	}

	{
		ZeroMemory(&mappedResource, sizeof(mappedResource));

		//LightBuffer
		LightBuffer lightBuffer = {};
		lightBuffer.ambientColor = aLight->GetAmbientColor();
		lightBuffer.diffuseColor = aLight->GetDiffuseColor();
		lightBuffer.lightDirection = aLight->GetDirection();
		lightBuffer.specularColor = aLight->GetSpecularColor();
		lightBuffer.specularPower = aLight->GetSpecularPower();

		aDeviceContext->Map(myLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &myLightBuffer, sizeof(LightBuffer));
		aDeviceContext->Unmap(myLightBuffer, 0);

		aDeviceContext->PSSetConstantBuffers(2, 1, &myLightBuffer);
	}

	{
		ZeroMemory(&mappedResource, sizeof(mappedResource));

		//CameraBuffer
		CameraBuffer cameraBufferData = {};
		cameraBufferData.cameraPosition = aCamera->GetViewMatrix().GetPosition();

		aDeviceContext->Map(myCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &myCameraBuffer, sizeof(CameraBuffer));
		aDeviceContext->Unmap(myCameraBuffer, 0);

		aDeviceContext->VSSetConstantBuffers(3, 1, &myCameraBuffer);
	}

	return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* aContext)
{
	aContext->IASetInputLayout(myLayout);
	aContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	aContext->VSSetShader(myVertexShader, NULL, 0);
	aContext->PSSetShader(myPixelShader, NULL, 0);

	aContext->PSSetSamplers(0, 1, &mySampleState);
}

bool LightShader::Render(ID3D11DeviceContext* aDeviceContext, Light* aLight, Camera* aCamera, Model* aModel)
{
	RenderShader(aDeviceContext);

	if (!UpdateAndBindBuffers(aDeviceContext, aLight, aCamera))
		return false;

	if (!SetObjectData(aDeviceContext, aModel))
		return false;

	aModel->Render(aDeviceContext);

	return true;
}

void LightShader::Shutdown()
{
	if (myCameraBuffer)
	{
		myCameraBuffer->Release();
		myCameraBuffer = nullptr;
	}
	if (myLightBuffer)
	{
		myLightBuffer->Release();
		myLightBuffer = nullptr;
	}

	if (myObjectBuffer)
	{
		myObjectBuffer->Release();
		myObjectBuffer = nullptr;
	}

	if (myFrameBuffer)
	{
		myFrameBuffer->Release();
		myFrameBuffer = nullptr;
	}

	if (mySampleState)
	{
		mySampleState->Release();
		mySampleState = nullptr;
	}

	if (myLayout)
	{
		myLayout->Release();
		myLayout = nullptr;
	}

	if (myPixelShader)
	{
		myPixelShader->Release();
		myPixelShader = nullptr;
	}

	if (myVertexShader)
	{
		myVertexShader->Release();
		myVertexShader = nullptr;
	}
}