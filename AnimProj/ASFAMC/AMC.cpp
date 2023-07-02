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

	// boneCount will be provided by Skeleton
	// int boneCount;

	int frameCount = 0;
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

		std::stringstream subStream(line);

		// the motion data order of each frame is same
		// so when read data of first frame, get the names of the bones
		subStream >> boneName;
		if (1 == frameCount)
			_dataOrder.push_back(boneName);

		float buffer;
		while (subStream >> buffer)
		{
			_data.push_back(buffer);
		}
	}

	return true;
}
