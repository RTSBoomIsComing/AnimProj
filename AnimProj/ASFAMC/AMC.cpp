// author: Changwan Yu
#include "pch.h"
#include "AMC.h"

pa::AMC::AMC(const wchar_t* filePath)
{
	std::cout << "TEST: Start AMC Construction\n";
	if (loadFromFile(filePath) != true)
		DebugBreak();
}

bool pa::AMC::loadFromFile(const wchar_t* filePath)
{
	std::ifstream stream{ filePath };
	if (stream.fail())
		return false;

	int frameCount = 0;
	int boneCount = 0;
	char line[200] = {};
	char boneName[100] = {};
	while (stream.getline(line, sizeof(line), '\n'))
	{
		if ('#' == line[0])
			continue;

		if (':' == line[0])
			continue;

		if (std::isdigit(line[0]))
		{
			frameCount++;
			continue;	// start motion data by frame
		}

		std::stringstream subStream{ line };

		subStream.getline(boneName, sizeof(boneName), ' ');
		boneCount++;
		// TODO
		std::cout << boneName << std::endl;

		char buffer[50] = {};
		while (subStream.getline(buffer, sizeof(buffer), ' '))
		{
			// TODO
			//std::cout << line << std::endl;
		}
	}

	boneCount /= frameCount;

	return true;
}
