// author: Changwan Yu
#include "pch.h"
#include "App/Win32Framework.h"
#include "MyApplication.h"
#include "Keyboard.h"

pa::Keyboard* GKeyboard = nullptr;

int main()
{
	GKeyboard = new pa::Keyboard();


	pa::MyApplication myApplication{};
	pa::Win32Framework myFramework{ &myApplication };
	int result = myFramework.Run();

	if (nullptr != GKeyboard)
	{
		delete GKeyboard;
		GKeyboard = nullptr;
	}

	return result;
}