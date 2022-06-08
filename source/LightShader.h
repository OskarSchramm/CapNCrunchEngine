//AUTHOR Oskar Schramm 2k22

#pragma once
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "CU/Matrix4x4.hpp"
#include "Model.h"
#include "Light.h"
#include "Camera.h"

using namespace std;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11SamplerState;
struct ID3D11Buffer;
struct ID3DDBlob;

class LightShader
{
#pragma region CBuffers
	struct FrameBuffer
	{
		CU::Matrix4x4f worldToClipMatrix;
	};
	struct ObjectBuffer
	{
		CU::Matrix4x4f modelMatrix;
	};
	struct CameraBuffer
	{
		CU::Vector3f cameraPosition;
		float padding;
	};
	struct LightBuffer
	{
		CU::Vector4f ambientColor;
		CU::Vector4f diffuseColor;
		CU::Vector3f lightDirection;
		float specularPower;
		CU::Vector4f specularColor;
	};
#pragma endregion
public:
	LightShader();
	~LightShader();

	bool Init(ID3D11Device* aDevice, HWND* aHandle);
	void Shutdown();
	bool Render(ID3D11DeviceContext* aDeviceContext, Light* aLight, Camera* aCamera, Model* aModel);
private:
	//Initializing
	bool InitShaders(ID3D11Device*, HWND*, WCHAR*, WCHAR*);
	bool SetPolygonLayout(ID3D11Device* const aDevice, ID3DBlob* aVertexShaderBlob);
	bool CreateConstantBuffers(ID3D11Device* aDevice);
	bool CreateSamplerState(ID3D11Device* aDevice);

	//Rendering
	bool SetObjectData(ID3D11DeviceContext* aDeviceContext, Model* aModel);
	bool UpdateAndBindBuffers(ID3D11DeviceContext* aDeviceContext, Light* aLight, Camera* aCamera);
	void RenderShader(ID3D11DeviceContext* aContext);

	//Helper Functions
	bool ReadShaderFile(HWND* anHWND, WCHAR* aShaderFilename, ID3DBlob** aShaderBlob, ID3DBlob** anErrorBlob);
	void OutputShaderErrorMessage(ID3DBlob*, HWND, WCHAR*);

	//Shader specifics
	ID3D11VertexShader* myVertexShader;
	ID3D11PixelShader*  myPixelShader;
	ID3D11InputLayout*  myLayout;
	ID3D11SamplerState* mySampleState;

	//Buffers
	ID3D11Buffer* myObjectBuffer;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myLightBuffer;
	ID3D11Buffer* myCameraBuffer;
};