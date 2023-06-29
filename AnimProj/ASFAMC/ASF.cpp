// author: Changwan Yu
#include "pch.h"
#include "ASF.h"

pa::ASF::ASF(const wchar_t* filePath)
{
	if (false == loadFromFile(filePath))
		DebugBreak();
}

bool pa::ASF::loadFromFile(const wchar_t* filePath)
{
	std::ifstream stream{ filePath };
	if (false == stream.is_open())
		return false;

	std::string line;
	while (stream)
	{
		std::getline(stream, line);

		if (0 == line.compare(":units"))
			parseUnits(stream);

		if (0 == line.compare(":root"))
			parseRoot(stream);

		if (0 == line.compare(":bonedata"))
			parseBoneData(stream);

		if (0 == line.compare(":hierarchy"))
			parseHierarchy(stream);
	}

	return true;
}

void pa::ASF::parseUnits(std::ifstream& stream)
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

void pa::ASF::parseRoot(std::ifstream& stream)
{
	std::cout << "Parse Root\n";
	_boneData.emplace_back();

	std::string buffer;
	for (int i = 0; i < 4; i++)
	{
		stream >> buffer;
		if (0 == buffer.compare("order"))
			parseAMCRootDataOrder(stream);

		if (0 == buffer.compare("axis"))
			parseAxisOrder(stream, _boneData[0]);

		if (0 == buffer.compare("position"))
			parseRootPosition(stream);

		if (0 == buffer.compare("orientation"))
			parseAxis(stream, _boneData[0]);
	}
}

void pa::ASF::parseBoneData(std::ifstream& stream)
{
	std::cout << "Parse BoneData\n";
	std::string buffer;
	while (stream)
	{
		stream >> buffer;
		if (buffer.find("begin") != std::string::npos)
		{
			_boneData.emplace_back();
			parseEachBone(stream, _boneData.back());
		}
		else
		{
			stream.seekg(-1 * buffer.size(), std::ios::cur);
			break;
		}
	}

}

void pa::ASF::parseEachBone(std::ifstream& stream, Bone& bone)
{
	std::cout << "\t\tPars Each Bone\n";
	std::string buffer;
	while (stream)
	{
		stream >> buffer;

		if (buffer.find("name") != std::string::npos)
		{
			stream >> bone.name;
		}

		if (buffer.find("direction") != std::string::npos)
		{
			float direction[4] = {};
			for (int i = 0; i < 3; i++)
			{
				stream >> direction[i];
			}
			bone.direction = DirectX::XMFLOAT4{ direction };
		}

		if (buffer.find("length") != std::string::npos)
			stream >> bone.length;

		if (buffer.find("axis") != std::string::npos)
		{
			parseAxis(stream, bone);
			parseAxisOrder(stream, bone);
		}

		if (buffer.find("dof") != std::string::npos)
		{
			parseDOF(stream, bone);
		}

		if (buffer.find("end") != std::string::npos)
			break;
	}
}

void pa::ASF::parseHierarchy(std::ifstream& stream)
{
	std::cout << "Parse Hierarchy\n";
}

void pa::ASF::parseAMCRootDataOrder(std::ifstream& stream)
{
	std::cout << "\tParse AMC Root Data Order\n";
	std::string buffer;
	for (int i = 0; i < 6; i++)
	{
		stream >> buffer;
		if (0 == buffer.compare("TX"))
			_rootOrder[0] = Bone::DOF::TX;

		if (0 == buffer.compare("TY"))
			_rootOrder[1] = Bone::DOF::TY;

		if (0 == buffer.compare("TZ"))
			_rootOrder[2] = Bone::DOF::TZ;

		if (0 == buffer.compare("RX"))
			_rootOrder[3] = Bone::DOF::RX;

		if (0 == buffer.compare("RY"))
			_rootOrder[4] = Bone::DOF::RY;

		if (0 == buffer.compare("RZ"))
			_rootOrder[5] = Bone::DOF::RZ;
	}

}

void pa::ASF::parseAxisOrder(std::ifstream& stream, Bone& bone)
{
	std::cout << "\tParse Rotation Axis Order\n";
	std::string buffer;
	stream >> buffer;
	if (0 == buffer.compare("XYZ"))
		bone.axisOrder = Bone::AxisOrder::XYZ;

	if (0 == buffer.compare("XZY"))
		bone.axisOrder = Bone::AxisOrder::XZY;

	if (0 == buffer.compare("YZX"))
		bone.axisOrder = Bone::AxisOrder::YZX;

	if (0 == buffer.compare("YXZ"))
		bone.axisOrder = Bone::AxisOrder::YXZ;

	if (0 == buffer.compare("ZXY"))
		bone.axisOrder = Bone::AxisOrder::ZXY;

	if (0 == buffer.compare("ZYX"))
		bone.axisOrder = Bone::AxisOrder::ZYX;
}

void pa::ASF::parseRootPosition(std::ifstream& stream)
{
	std::cout << "\tParse Root Position\n";
	float position[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	for (int i = 0; i < 3; i++)
	{
		stream >> position[i];
	}
	_rootPosition = DirectX::XMFLOAT4{ position };
}

void pa::ASF::parseAxis(std::ifstream& stream, Bone& bone)
{
	std::cout << "\tParse Root Position\n";
	float axis[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // rotation.w have to be not used
	for (int i = 0; i < 3; i++)
	{
		stream >> axis[i];
	}
	bone.axis = DirectX::XMFLOAT4{ axis };


}

void pa::ASF::parseDOF(std::ifstream& stream, Bone& bone)
{
	std::cout << "\tParse DOF\n";
	std::string buffer;
	int dofId = 0;
	while (stream)
	{
		stream >> buffer;

		Bone::DOF dof = {};
		if (0 == buffer.compare("rx"))
			dof = Bone::DOF::RX;
		else if (0 == buffer.compare("ry"))
			dof = Bone::DOF::RY;
		else if (0 == buffer.compare("rz"))
			dof = Bone::DOF::RZ;
		else if (0 == buffer.compare("ln"))
			dof = Bone::DOF::LN;
		else
		{
			stream.seekg(-1 * buffer.size(), std::ios::cur);
			break;
		}

		bone.dof[dofId] = dof;
		dofId++;
	}
}
