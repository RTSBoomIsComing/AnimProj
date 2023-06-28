// author: Changwan Yu
#include "pch.h"
#include "App/Win32Framework.h"
#include "MyApplication.h"
int main()
{
	pa::MyApplication myApplication{};
	pa::Win32Framework myFramework{ &myApplication };

	return myFramework.Run();
}