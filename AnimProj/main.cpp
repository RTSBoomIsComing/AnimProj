// author: Changwan Yu
#include "pch.h"
#include "App/Win32Framework.h"
#include "MyApplication.h"
#include "Keyboard.h"
#include "Animation/AnimationManager.h"


int main()
{
	pa::MyApplication myApplication{};
	pa::Win32Framework myFramework{ &myApplication };
	int result = myFramework.Run();

	return result;
}