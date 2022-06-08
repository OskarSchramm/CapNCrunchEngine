#pragma once
#include "CustomImGui.h"
#include "ImGuiInfo.h"
#include <Windows.h> //Try to remove and forward declare HWND

struct ID3D11Device;
struct ID3D11DeviceContext;
class  ModelFactory; 

class ImGuiInterface
{
public:
	ImGuiInterface();
	~ImGuiInterface();

	bool Init(HWND* aHwnd, ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext);
	void NewFrame();
	void Render();
	void ShutDown();

	CustomImGui& GetCustomImGui() { return myCustomImGui; }
	void SendCustomInfo(CustomImGuiInfo someInfo);
	void SetGraphicsEngine(GraphicsEngine* aGFX) { myCustomImGui.myGraphicsEngineptr = aGFX; }
private:
	CustomImGui myCustomImGui;
};