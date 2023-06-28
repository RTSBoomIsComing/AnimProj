// author: Changwan Yu
#include "pch.h"
#include "ASF.h"

pa::ASF::ASF(const wchar_t* filePath)
{
	if (false == loadASFFromFile(filePath))
		DebugBreak();
}

bool pa::ASF::loadASFFromFile(const wchar_t* filePath)
{
	std::ifstream inputStream{ filePath };
	if (false == inputStream.is_open())
		return false;

	std::string line;
	line.reserve();

	while (true != inputStream.eof())
	{
		std::getline(inputStream, line);
		std::cout << line << std::endl;
	}

	return true;
}
