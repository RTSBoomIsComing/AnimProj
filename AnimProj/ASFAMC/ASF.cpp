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
	std::ifstream stream{ filePath };
	if (false == stream.is_open())
		return false;

	std::string line;
	line.reserve();

	while (true != stream.eof())
	{
		std::getline(stream, line);

		if (0 == line.compare(":units"))
			ParseUnits(stream);

		if (0 == line.compare(":root"))
			ParseRoot(stream);

		if (0 == line.compare(":bonedata"))
			ParseBoneData(stream);

		if (0 == line.compare(":hierarchy"))
			ParseHierarchy(stream);
	}

	return true;
}

void pa::ASF::ParseUnits(std::ifstream& stream)
{
	std::cout << "Parse Units\n";
	for (int i = 0; i < 3; i++)
	{
		std::string unitName;
		stream >> unitName;
		if (0 == unitName.compare("mass"))
			stream >> _unitMass;

		if (0 == unitName.compare("length"))
			stream >> _unitLength;

		if (0 == unitName.compare("angle"))
		{
			std::string unitAngleString;
			unitAngleString.reserve(3);

			stream >> unitAngleString;
			_unitAngle = (0 == unitAngleString.compare("deg")) ? UnitAngle::Degree : UnitAngle::Radian;
		}
	}
}

void pa::ASF::ParseRoot(std::ifstream& stream)
{
	std::cout << "Parse Root\n";

}

void pa::ASF::ParseBoneData(std::ifstream& stream)
{
	std::cout << "Parse BoneData\n";
}

void pa::ASF::ParseHierarchy(std::ifstream& stream)
{
	std::cout << "Parse Hierarchy\n";
}
