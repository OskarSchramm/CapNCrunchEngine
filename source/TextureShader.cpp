//AUTHOR Oskar Schramm 2k22

#include "TextureShader.h"

TextureShader::TextureShader()
	: myVertexShader(nullptr), myPixelShader(nullptr), myLayout(nullptr), myMatrixBuffer(nullptr), mySampleState(nullptr)
{}

TextureShader::~TextureShader() {}

bool TextureShader::Initialize(ID3D11Device* aDevice, HWND aHWND)
{
	bool result;

	result = InitializeShader(aDevice, aHWND, (WCHAR*)L"source/shaders/textureVS.hlsl", (WCHAR*)L"source/shaders/texturePS.hlsl");
	if (!result)
		return false;

	return true;
}

void TextureShader::Shutdown()
{
	ShutdownShader();

	return;
}

bool TextureShader::Render(ID3D11DeviceContext* aDeviceContext, int aIndexCount,
	DirectX::XMMATRIX aWorldMatrix, DirectX::XMMATRIX aViewMatrix, DirectX::XMMATRIX aProjectionMatrix,
	ID3D11ShaderResourceView* aTexture)
{
	bool result;

	result = SetShaderParameters(aDeviceContext, aWorldMatrix, aViewMatrix, aProjectionMatrix, aTexture);
	if (!result)
		return false;

	RenderShader(aDeviceContext, aIndexCount);

	return true;
}

bool TextureShader::InitializeShader(ID3D11Device* aDevice, HWND aHWND, WCHAR* aVSFilename, WCHAR* aPSFilename)
{
	HRESULT result;
	ID3D10Blob* errorMSG = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	D3D11_SAMPLER_DESC samplerDesc;

	result = D3DCompileFromFile(aVSFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMSG);
	if (FAILED(result))
	{
		result = D3DReadFileToBlob(L"TextureVS.cso", &vertexShaderBuffer);
		if (FAILED(result))
		{
			if (errorMSG)
				OutputShaderErrorMessage(errorMSG, aHWND, aVSFilename);
			else
				MessageBox(aHWND, aVSFilename, L"Missing Shader File", MB_OK);

			return false;
		}
	}

	result = D3DCompileFromFile(aPSFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMSG);
	if (FAILED(result))
	{
		result = D3DReadFileToBlob(L"TexturePS.cso", &pixelShaderBuffer);
		if (FAILED(result))
		{
			if (errorMSG)
				OutputShaderErrorMessage(errorMSG, aHWND, aPSFilename);
			else
				MessageBox(aHWND, aVSFilename, L"Missing Shader File", MB_OK);

			return false;
		}
	}

	result = aDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &myVertexShader);
	if (FAILED(result))
		return false;

	result = aDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &myPixelShader);
	if (FAILED(result))
		return false;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = aDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &myLayout);
	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = aDevice->CreateBuffer(&matrixBufferDesc, NULL, &myMatrixBuffer);
	if (FAILED(result))
		return false;

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

	result = aDevice->CreateSamplerState(&samplerDesc, &mySampleState);
	if (FAILED(result))
		return false;

	return true;
}

void TextureShader::ShutdownShader()
{
	if (mySampleState)
	{
		mySampleState->Release();
		mySampleState = 0;
	}

	if (myMatrixBuffer)
	{
		myMatrixBuffer->Release();
		myMatrixBuffer = nullptr;
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

	return;
}

void TextureShader::OutputShaderErrorMessage(ID3D10Blob* anErrorMSG, HWND aHWND, WCHAR* aShaderFilename)
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

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* aDeviceContext,
	DirectX::XMMATRIX aWorldMatrix, DirectX::XMMATRIX aViewMatrix, DirectX::XMMATRIX aProjectionMatrix,
	ID3D11ShaderResourceView* aTexture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	aWorldMatrix = DirectX::XMMatrixTranspose(aWorldMatrix);
	aViewMatrix = DirectX::XMMatrixTranspose(aViewMatrix);
	aProjectionMatrix = DirectX::XMMatrixTranspose(aProjectionMatrix);

	result = aDeviceContext->Map(myMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = aWorldMatrix;
	dataPtr->view = aViewMatrix;
	dataPtr->projection = aProjectionMatrix;

	aDeviceContext->Unmap(myMatrixBuffer, 0);

	bufferNumber = 0;

	aDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &myMatrixBuffer);

	aDeviceContext->PSSetShaderResources(0, 1, &aTexture);

	return true;
}

void TextureShader::RenderShader(ID3D11DeviceContext* aDeviceContext, int aIndexCount)
{
	aDeviceContext->IASetInputLayout(myLayout);

	aDeviceContext->VSSetShader(myVertexShader, NULL, 0);
	aDeviceContext->PSSetShader(myPixelShader, NULL, 0);

	aDeviceContext->PSSetSamplers(0, 1, &mySampleState);

	aDeviceContext->DrawIndexed(aIndexCount, 0, 0);

	return;
}