//AUTHOR Oskar Schramm 2k22

#pragma once

//wnds
#include <Windows.h>

//cpc
#include "Engine.h"
#include "GraphicsEngine.h"
#include "ImGuiInterface.h"

class SystemClass
{
public:
	SystemClass();
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
private:
	void InitalizeWindows(int&, int&);
	void ShutDownWindows();

	bool UpdateFrame();
	bool RenderFrame();

	bool myIsRunning;

	LPCWSTR myApplicationName;
	HINSTANCE myHInstance;
	HWND myHWND;

	GraphicsEngine  myGraphics;
	Engine          myEngine;
	ImGuiInterface  myImgui;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle = 0;