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
	if (stream.is_open() != true)
		return false;

	while (stream)
	{
		std::string line;
		std::getline(stream, line);
		if (std::isdigit(stream.peek()))
		{
			std::string line;
			std::getline(stream, line); // Discard frameIndex
			while (stream && std::isdigit(stream.peek()) != true)
			{
				std::getline(stream, line);

				std::istringstream subStream{ line };

				std::string boneName;
				subStream >> boneName;
				std::cout << boneName << std::endl;
				float val{};
				while (subStream >> val)
				{
					//std::cout << val << std::endl;
				}
			}
		}
	}

	return true;
}
