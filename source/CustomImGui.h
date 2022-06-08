//AUTHOR Oskar Schramm 2k22

#pragma once
#include "ImGuiInfo.h"
#include <type_traits>
#include <vector>
#include <string>

namespace
{
	enum class DebugFeature //GraphicsDebugFeature
	{
		None = 0,
		TrackAllAllocations = 1 << 1,
		TrackStackTraces = 1 << 2,
		GPUInfo = 1 << 3,
		CPUInfo = 1 << 4,
		Profiler = 1 << 5,
		All = (1 << 6) - 1,
	};

	inline DebugFeature operator|(DebugFeature lhs, DebugFeature rhs)
	{
		return static_cast<DebugFeature>(static_cast<std::underlying_type<DebugFeature>::type>(lhs) | static_cast<std::underlying_type<DebugFeature>::type>(rhs));
	}
	inline DebugFeature& operator|=(DebugFeature& lhs, DebugFeature rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}
	inline DebugFeature operator&(DebugFeature lhs, DebugFeature rhs)
	{
		return static_cast<DebugFeature>(static_cast<std::underlying_type<DebugFeature>::type>(lhs) & static_cast<std::underlying_type<DebugFeature>::type>(rhs));
	}
	inline DebugFeature operator~(DebugFeature lhs)
	{
		return static_cast<DebugFeature>(~static_cast<std::underlying_type<DebugFeature>::type>(lhs));
	}
	inline DebugFeature operator&=(DebugFeature lhs, DebugFeature rhs)
	{
		lhs = lhs & rhs;
		return lhs;
	}
}

class GraphicsEngine;

class CustomImGui
{
	friend class ImGuiInterface;
public:
	CustomImGui();
	~CustomImGui();
	void Init();
	void Render();
	void ShutDown();

	void GetCustomInfo(CustomImGuiInfo someInfo);

	char* GetAdapterName() { return myAdapterName; };
	int* GetDVram() { return myDVM; };
private:
	bool IsDebugFeatureOn(DebugFeature aFeature) const;

	void SetupBGDockSpace();
	void NavigationBar();
	void ProfilerWindow();
	void GPUInfo();

	void Filter();
	
	void ModelLoader();
private:
	char* myAdapterName;
	int* myDVM;

	DebugFeature myDebugFeatures;

	std::vector<std::string>  myModelFilePaths;
	std::vector<std::string>  myModelsButtons;

	GraphicsEngine* myGraphicsEngineptr;
};