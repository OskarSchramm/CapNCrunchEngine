#pragma once
#include "CU/Timer.h"
#include "CU/Input.h"

class Engine
{
public:
	Engine();
	~Engine();
	bool Init(HWND*);

	void Update();

	inline CU::Input* GetInput() { return &myInput; }

	inline double GetDeltaTime() { return myTimer.GetDeltaTime(); }
	inline float  GetTotalTime() { return myTimer.GetTotalTime(); }
private:
	CU::Timer myTimer;
	CU::Input myInput;
};