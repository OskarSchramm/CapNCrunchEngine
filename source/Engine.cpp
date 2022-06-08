#include "Engine.h"

Engine::Engine() = default;
Engine::~Engine() = default;

bool Engine::Init(HWND* aWindowHandle)
{
	if (!myInput.Init(aWindowHandle))
		return false;

	return true;
}

void Engine::Update()
{
	myTimer.Update();
}