//AUTHOR Oskar Schramm 2k22
#include "SystemClass.h"

//ext
#include "imgui/imgui.h"

SystemClass::SystemClass()
	: myApplicationName(nullptr), myHInstance(nullptr), myHWND(nullptr), myIsRunning(true)
{}

SystemClass::~SystemClass() {}

bool SystemClass::Initialize()
{
	int screenWidth = 0;
	int screenHeight = 0;
	bool result;

	InitalizeWindows(screenWidth, screenHeight);

	//Engine
	result = myEngine.Init(&myHWND);
	if (!result)
		return false;

	//GFX
	result = myGraphics.Initialize(myHWND, screenWidth, screenHeight);
	if (!result)
		return false;

	myGraphics.SetInput(myEngine.GetInput());

	//Imgui
	result = myImgui.Init(&myHWND, myGraphics.GetD3DClass()->GetDevice(), myGraphics.GetD3DClass()->GetDeviceContext());
	if (!result)
		return false;

	//Set imgui gfx and dvr 
	myGraphics.GetD3DClass()->GetVideoCardInfo(myImgui.GetCustomImGui().GetAdapterName(), *myImgui.GetCustomImGui().GetDVram());
	myImgui.SetGraphicsEngine(&myGraphics);

	return true;
}

void SystemClass::Shutdown()
{
	myGraphics.Shutdown();

	ShutDownWindows();

	myImgui.ShutDown();

	return;
}

void SystemClass::Run()
{
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	while (myIsRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			myIsRunning = false;
		else
		{
			if (!UpdateFrame())
				myIsRunning = false;

			if(!RenderFrame())
				myIsRunning = false;
		}
	}
}

bool SystemClass::UpdateFrame()
{
	//Engine
	myEngine.Update();

	float deltaTime = myEngine.GetDeltaTime();

	//GFX
	myGraphics.Update(deltaTime);

	return true;
}

bool SystemClass::RenderFrame()
{
	myImgui.NewFrame();
	myGraphics.GetD3DClass()->BeginFrame(0.0f, 0.0f, 0.3f, 1.0f);

	//GFX
	myGraphics.Render();

	myImgui.Render();

	//Present
	myGraphics.GetD3DClass()->EndFrame();

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND aHwnd, UINT aUmsg, WPARAM aWparam, LPARAM aLparam)
{
	if (myEngine.GetInput()->UpdateEvents(aUmsg, aWparam, aLparam))
		return 0;

	switch (aUmsg)
	{
		default:
		{
			return DefWindowProc(aHwnd, aUmsg, aWparam, aLparam);
		}
	}
}

void SystemClass::InitalizeWindows(int& screenWidth, int& screenHeight)
{
	DEVMODE dmScreenSettings;
	int posX, posY;

	ApplicationHandle = this;

	myHInstance = GetModuleHandle(NULL);

	myApplicationName = L"CapNCrunch Engine";

	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = myHInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = myApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = 0;
		posY = 0;
	}
	else
	{
		screenWidth = 1280;
		screenHeight = 720;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	myHWND = CreateWindowEx(WS_EX_APPWINDOW, myApplicationName, myApplicationName,
		WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, myHInstance, NULL);

	ShowWindow(myHWND, SW_SHOW);
	SetForegroundWindow(myHWND);
	SetFocus(myHWND);

	ShowCursor(true);

	return;
}

void SystemClass::ShutDownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(myHWND);
	myHWND = NULL;

	UnregisterClass(myApplicationName, myHInstance);
	myHInstance = NULL;

	ApplicationHandle = NULL;

	return;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND aHwnd, UINT aUMessage, WPARAM aWparam, LPARAM aLparam)
{
	if (ImGui_ImplWin32_WndProcHandler(aHwnd, aUMessage, aWparam, aLparam))
		return S_OK;

	switch (aUMessage)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return ApplicationHandle->MessageHandler(aHwnd, aUMessage, aWparam, aLparam);
		}
	}
}