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
	_boneData.emplace_back();

	std::string buffer;
	for (int i = 0; i < 4; i++)
	{
		stream >> buffer;
		if (0 == buffer.compare("order"))
			ParseAMCRootDataOrder(stream);

		if (0 == buffer.compare("axis"))
			ParseRotationAxisOrder(stream, _boneData.back());

		if (0 == buffer.compare("position"))
			ParseRootPosition(stream);
	}
}

void pa::ASF::ParseBoneData(std::ifstream& stream)
{
	std::cout << "Parse BoneData\n";
}

void pa::ASF::ParseHierarchy(std::ifstream& stream)
{
	std::cout << "Parse Hierarchy\n";
}

void pa::ASF::ParseAMCRootDataOrder(std::ifstream& stream)
{
	std::cout << "\tParse AMC Root Data Order\n";
	std::string buffer;
	for (int i = 0; i < 6; i++)
	{
		stream >> buffer;
		if (0 == buffer.compare("TX"))
			_amcRootOrder[0] = i;

		if (0 == buffer.compare("TY"))
			_amcRootOrder[1] = i;

		if (0 == buffer.compare("TZ"))
			_amcRootOrder[2] = i;

		if (0 == buffer.compare("RX"))
			_amcRootOrder[3] = i;

		if (0 == buffer.compare("RY"))
			_amcRootOrder[4] = i;

		if (0 == buffer.compare("RZ"))
			_amcRootOrder[5] = i;
	}

}

void pa::ASF::ParseRotationAxisOrder(std::ifstream& stream, Bone& bone)
{
	std::cout << "\tParse Rotation Axis Order\n";
	std::string buffer;
	stream >> buffer;
	if (0 == buffer.compare("XYZ"))
		bone.axis = Bone::RotationAxisOrder::XYZ;

	if (0 == buffer.compare("XZY"))
		bone.axis = Bone::RotationAxisOrder::XZY;

	if (0 == buffer.compare("YZX"))
		bone.axis = Bone::RotationAxisOrder::YZX;

	if (0 == buffer.compare("YXZ"))
		bone.axis = Bone::RotationAxisOrder::YXZ;

	if (0 == buffer.compare("ZXY"))
		bone.axis = Bone::RotationAxisOrder::ZXY;

	if (0 == buffer.compare("ZYX"))
		bone.axis = Bone::RotationAxisOrder::ZYX;
}

void pa::ASF::ParseRootPosition(std::ifstream& stream)
{
	std::cout << "\tParse Root Position\n";
	float position[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	for (int i = 0; i < 3; i++)
	{
		stream >> position[i];
	}
	_rootPosition = DirectX::XMFLOAT4{ position };
}
