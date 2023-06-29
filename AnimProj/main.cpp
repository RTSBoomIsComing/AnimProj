// author: Changwan Yu
#include "pch.h"
#include "App/Win32Framework.h"
#include "MyApplication.h"

#include "ASFAMC/AMC.h"
#include "ASFAMC/ASF.h"

#include <iostream>

int main()
{
	pa::MyApplication myApplication{};
	pa::Win32Framework myFramework{ &myApplication };

	return myFramework.Run();
}