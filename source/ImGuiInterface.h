#pragma once
#include "ImGuiInfo.h"
#include <Windows.h> //Try to remove and forward declare HWND
#include "CustomImGui.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
class  ModelFactory; 

struct ImGuiIO;

enum class ImguiFont
{
	Regular,
	Bold,
	Italic
};

class ImGuiInterface
{
public:
	ImGuiInterface();
	~ImGuiInterface();

	bool Init(HWND* aHwnd, ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext);
	void NewFrame();
	void Render();
	void ShutDown();

	inline CustomImGui* GetCustomImGui() { return myCustomImGui; }
	void SendCustomInfo(CustomImGuiInfo someInfo);
	void SetGraphicsEngine(GraphicsEngine* aGFX) { myCustomImGui->myGraphicsEngineptr = aGFX; }
private:
	void SetDarkThemeColors();
	void SetupFonts(ImGuiIO& io);

	CustomImGui* myCustomImGui;
};