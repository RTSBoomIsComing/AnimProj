// author: Changwan Yu
#include "pch.h"
#include "AMC.h"
#include "ASF.h"
#include "../Animation/Skeleton.h"

pa::AMC::AMC(const wchar_t* filePath)
{
	if (loadFromFile(filePath) != true)
		DebugBreak();
}

pa::AMC::AMC(const std::wstring& filePath)
{
	if (loadFromFile(filePath.c_str()) != true)
		DebugBreak();
}

bool pa::AMC::loadFromFile(const wchar_t* filePath)
{
	std::ifstream stream{ filePath };
	if (stream.fail())
		return false;

	// boneCount will be provided by Skeleton
	// int boneCount;

	_frameCount = 0;
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
			_frameCount++;
			continue;	// start motion data by frame
		}

		std::stringstream subStream(line);

		// the motion data order of each frame is same
		// so when read data of first frame, get the names of the bones
		subStream >> boneName;
		if (1 == _frameCount)
			_boneNames.push_back(boneName);

		float buffer;
		while (subStream >> buffer)
		{
			_data.push_back(buffer);
		}
	}

	return true;
}
