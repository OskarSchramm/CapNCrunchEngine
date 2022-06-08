//AUTHOR Oskar Schramm 2k22

#include <Windows.h>
#include "SystemClass.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SystemClass* system;
	bool result;

	system = new SystemClass;
	if (!system)
		return 0;

	result = system->Initialize();
	if (result)
	{
		system->Run();
	}

	system->Shutdown();
	delete system;
	system = nullptr;
	return 0;
}