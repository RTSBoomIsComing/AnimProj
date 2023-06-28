// author: Changwan Yu
#include "pch.h"
#include "App/Win32Framework.h"
#include "MyApplication.h"

#include "ASFAMC/AMC.h"
#include "ASFAMC/ASF.h"

#include <iostream>

int main()
{
	//std::wstring asfFilePath = _SOLUTIONDIR;
	//asfFilePath += LR"(Assets\ASFAMC\07-walk\07-walk.asf)";
	//pa::ASF asf(asfFilePath.c_str());

	pa::MyApplication myApplication{};
	pa::Win32Framework myFramework{ &myApplication };

	return myFramework.Run();
}