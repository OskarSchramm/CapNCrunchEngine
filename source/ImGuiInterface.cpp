#include "ImGuiInterface.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <string>

ImGuiInterface::ImGuiInterface() {}
ImGuiInterface::~ImGuiInterface() {}

bool ImGuiInterface::Init(HWND* aHwnd, ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = "imgui.ini";
	io.LogFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	if (!ImGui_ImplWin32_Init(*aHwnd))
		return false;

	if(!ImGui_ImplDX11_Init(aDevice, aDeviceContext))
		return false;

	ImGui::StyleColorsDark();

	myCustomImGui.Init();

	return true;
}

void ImGuiInterface::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiInterface::Render()
{
	//RENDER CUSTOM IMGUI CLASS(ES)
	myCustomImGui.Render();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiInterface::ShutDown()
{
	myCustomImGui.ShutDown();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiInterface::SendCustomInfo(CustomImGuiInfo someInfo)
{
	myCustomImGui.GetCustomInfo(someInfo);
}