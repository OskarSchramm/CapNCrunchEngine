#include "ImGuiInterface.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <string>

ImGuiInterface::ImGuiInterface() 
{
	myCustomImGui = new CustomImGui(this);
}

ImGuiInterface::~ImGuiInterface()
{
	delete myCustomImGui;
	myCustomImGui = nullptr;
}

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
	SetDarkThemeColors();
	SetupFonts(io);

	myCustomImGui->Init();

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
	myCustomImGui->Render();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiInterface::ShutDown()
{
	myCustomImGui->ShutDown();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiInterface::SendCustomInfo(CustomImGuiInfo someInfo)
{
	myCustomImGui->GetCustomInfo(someInfo);
}

void ImGuiInterface::SetDarkThemeColors()
{
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

	//Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	//Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	//Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	//Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.15f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	//
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.95f, 0.1505f, 0.95f, 1.0f };
}

void ImGuiInterface::SetupFonts(ImGuiIO& io)
{
	auto defFont = io.Fonts->AddFontFromFileTTF("bin/data/fonts/opensans/OpenSans-Regular.ttf", 18.0f); //Regular 0
	io.FontDefault = defFont;

	io.Fonts->AddFontFromFileTTF("bin/data/fonts/opensans/OpenSans-Bold.ttf", 18.0f); //BOLD 1
	io.Fonts->AddFontFromFileTTF("bin/data/fonts/opensans/OpenSans-Italic.ttf", 18.0f); //Italic 2
}