// author: Changwan Yu
#include "pch.h"
#include "AMC.h"

pa::AMC::AMC()
{
	std::cout << "TEST: Start AMC Construction\n";

	
}

pa::AMC::AMC(const wchar_t* filePath)
{
	std::cout << "TEST: Start AMC Construction\n";
	loadFromFile(filePath);
}

bool pa::AMC::loadFromFile(const wchar_t* filePath)
{
	return false;
}
