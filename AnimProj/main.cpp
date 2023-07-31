// author: Changwan Yu
#include "pch.h"
#include "App/Win32Framework.h"
#include "MyApplication.h"
#include "Animation/AnimationManager.h"
#include <DirectXTK/Keyboard.h>
#include <DirectXTK/Mouse.h>

int main()
{
	std::unique_ptr<DirectX::Keyboard> keyboard = std::make_unique<DirectX::Keyboard>();
	std::unique_ptr<DirectX::Mouse> mouse = std::make_unique<DirectX::Mouse>();
	

	pa::MyApplication myApplication{};
	pa::Win32Framework myFramework{ &myApplication };
	int result = myFramework.Run();

	return result;
}