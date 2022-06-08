//AUTHOR Oskar Schramm 2k22

#include "CustomImGui.h"
#include "imgui/imgui.h"
#include <stdio.h>
#include <filesystem>
#include "GraphicsEngine.h"

CustomImGui::CustomImGui()
{
	myDebugFeatures = DebugFeature::Profiler;
}

CustomImGui::~CustomImGui()
{
	ShutDown();
}

void CustomImGui::Init()
{
	myAdapterName = new char[128];
	myDVM = new int;

	//Load in all models in the models map into a string arrays
	{
		for (const auto& entry : std::filesystem::directory_iterator("bin/data/models/fbx"))
		{
			std::string entryString = entry.path().string();
			myModelFilePaths.push_back(entryString);

			std::string baseFilename = entryString.substr(entryString.find_last_of("\\") + 1);
			size_t fbxStart = baseFilename.find(".fbx");
			size_t FBXStart = baseFilename.find(".FBX");
			baseFilename = baseFilename.substr(0, fbxStart);
			baseFilename = baseFilename.substr(0, FBXStart);
			std::string btnName(baseFilename.begin(), baseFilename.end());
			myModelsButtons.push_back(btnName);
		}
	}
}

void CustomImGui::SetupBGDockSpace()
{
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("CapNCrunch Dockspace", nullptr, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 250.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	style.WindowMinSize.x = minWinSizeX;
}

void CustomImGui::Render()
{
	SetupBGDockSpace();
	NavigationBar();

	if (IsDebugFeatureOn(DebugFeature::Profiler))
		ProfilerWindow();

	if (IsDebugFeatureOn(DebugFeature::GPUInfo))
		GPUInfo();

	ModelLoader();

	ImGui::End();
}

void CustomImGui::ShutDown()
{
	delete[] myAdapterName;
	myAdapterName = nullptr;
	delete myDVM;
	myDVM = nullptr;
}

void CustomImGui::GetCustomInfo(CustomImGuiInfo someInfo)
{

}

void CustomImGui::NavigationBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Engine"))
		{
			if (ImGui::MenuItem("Graphics"))
			{
				if (IsDebugFeatureOn(DebugFeature::Profiler))
					myDebugFeatures |= DebugFeature::Profiler;
				else
					myDebugFeatures &= ~DebugFeature::Profiler;
			}

			if (ImGui::MenuItem("GPUInfo"))
			{
				if (IsDebugFeatureOn(DebugFeature::GPUInfo))
					myDebugFeatures |= DebugFeature::GPUInfo;
				else
					myDebugFeatures &= ~DebugFeature::GPUInfo;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("HELP"))
		{
			if (ImGui::MenuItem("Help"))
			{
				ImGui::Begin("HelpWindow");
				ImGui::Text("Remember this is a WIP! Any requests or feedback is helpfull!");
				ImGui::Text("If you need any help or have any specific requests, be sure to email me at: oskar.schramm01@gmail.com.");
				ImGui::End();
			}
			ImGui::EndMenu();
		}

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 2.0f);
		ImGui::Text("CapNCrunch Engine");

		ImGui::EndMenuBar();
	}
}

void CustomImGui::ProfilerWindow()
{
	ImGui::Begin("Profiler Window");

	Filter();

	ImGui::End();
}

void CustomImGui::Filter()
{
	static float values[90] = {};
	static int values_offset = 0;
	char overlay[32];
	static double refresh_time = 0.0;
	if (true || refresh_time == 0.0)
		refresh_time = ImGui::GetTime();
	while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
	{
		static float phase = 0.0f;
		values[values_offset] = cosf(phase);
		values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
		phase += 0.10f * values_offset;
		refresh_time += 1.0f / 60.0f;
	}

	{
		float average = 0.0f;
		for (int n = 0; n < IM_ARRAYSIZE(values); n++)
			average += values[n];
		average /= (float)IM_ARRAYSIZE(values);
		char overlay[32];
		ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
	}


	static ImGuiTextFilter filter;
	filter.Draw();

	const char* lines[] = { "fps", "gpu", "cpu", "render", "material", "present", "light", };
	for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
		if (filter.PassFilter(lines[i]))
			ImGui::BulletText("%s", lines[i]);
}

void CustomImGui::GPUInfo()
{
	ImGui::Begin("GPUInfo");
	char adapterOut[128];
	sprintf_s(adapterOut, "Adapter: ");
	strcat_s(adapterOut, myAdapterName);

	ImGui::Text(adapterOut);

	char dvrOut[128];
	sprintf_s(dvrOut, "DVR    : ");
	int i = *myDVM;
	char dvm[8];
	sprintf_s(dvm, "%d", i);
	strcat_s(dvrOut, dvm);

	ImGui::Text(dvrOut);
	ImGui::End();
}

void CustomImGui::ModelLoader()
{
	ImGui::Begin("ModelLoader");

	for (size_t i = 0; i < myModelsButtons.size(); i++)
	{
		auto& currBtn = myModelsButtons[i];

		if (ImGui::Button(currBtn.c_str()))
		{
			auto& currFilePath = myModelFilePaths[i];
			myGraphicsEngineptr->CreateModel(currFilePath.c_str());
		}
	}

	ImGui::End();
}

bool CustomImGui::IsDebugFeatureOn(DebugFeature aFeature) const
{
	const bool all = ((myDebugFeatures & DebugFeature::All) != DebugFeature::None);
	if (all)
	{
		return true;
	}

	const bool specific = ((myDebugFeatures & aFeature) != DebugFeature::None);
	return specific;
}