// author: Changwan Yu
#include "pch.h"
#include "App/Win32Framework.h"
#include "MyApplication.h"
#include "Keyboard.h"
#include "Animation/AnimationManager.h"
#include "TwoBoneIKSolver.h"

int main()
{
	using namespace DirectX;
	pa::TwoBoneIKSolver ikTest;
	ikTest.solve({ 0, 0, 0 }, { -1, 0, 0 }, { -1, 1, 0 }, { 1, 0, 0 }, { 0,0,0,1 }, { 0,0,0,1 });


	pa::MyApplication myApplication{};
	pa::Win32Framework myFramework{ &myApplication };
	int result = myFramework.Run();

	return result;
}