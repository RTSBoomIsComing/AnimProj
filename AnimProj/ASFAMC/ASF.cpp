// author: Changwan Yu
#include "pch.h"
#include "ASF.h"

pa::ASF::ASF(const wchar_t* filePath)
{
	if (loadFromFile(filePath) != true)
		DebugBreak();

	_globalRotations.resize(_boneData.size());
	for (int i = 0; i < _dfsRoute.size(); i++)
	{
		using namespace DirectX;

		const int boneIndex = _dfsRoute[i];
		Bone& bone = _boneData[boneIndex];

		const XMMATRIX globalRotation = EulerRotation(bone.axis, bone.axisOrder);
		_globalRotations[boneIndex] = globalRotation;
	}
}

bool pa::ASF::loadFromFile(const wchar_t* filePath)
{
	std::ifstream stream{ filePath };
	if (stream.fail())
		return false;

	std::string line;
	while (std::getline(stream, line))
	{
		if (0 == line.compare(":units"))
		{
			parseUnits(stream);
		}
		else if (0 == line.compare(":root"))
		{
			parseRoot(stream);
		}
		else if (0 == line.compare(":bonedata"))
		{
			parseBoneData(stream);
		}
		else if (0 == line.compare(":hierarchy"))
		{
			parseHierarchy(stream);
		}
	}

	return true;
}

void pa::ASF::parseUnits(std::ifstream& stream)
{
	std::string buffer;
	for (int i = 0; i < 3; i++)
	{
		stream >> buffer;
		if (0 == buffer.compare("mass"))
		{
			stream >> _unit.mass;
		}
		else if (0 == buffer.compare("length"))
		{
			stream >> _unit.length;
		}
		else if (0 == buffer.compare("angle"))
		{
			stream >> buffer;
			_unit.angle = (0 == buffer.compare("deg")) ? (DirectX::XM_PI / 180) : 1.0f;
		}
	}
	stream.ignore();
}

void pa::ASF::parseRoot(std::ifstream& stream)
{
	Bone bone = {};

	std::string buffer;
	for (int i = 0; i < 4; i++)
	{
		stream >> buffer;
		if (0 == buffer.compare("order"))
		{
			for (int j = 0; j < 6; j++)
			{
				stream >> _rootOrders[j];
			}
		}
		else if (0 == buffer.compare("axis"))
		{
			stream >> bone.axisOrder;
		}
		else if (0 == buffer.compare("position"))
		{
			stream >> _rootPosition.x >> _rootPosition.y >> _rootPosition.z;
		}
		else if (0 == buffer.compare("orientation"))
		{
			stream >> bone.axis.x >> bone.axis.y >> bone.axis.z;
		}
	}

	stream.ignore();
	_boneData.push_back(bone);
	_boneNameList.push_back("root");
}

void pa::ASF::parseBoneData(std::ifstream& stream)
{
	std::string buffer;
	while (stream && stream.peek() != ':')
	{
		if (stream.peek() == ' ' || stream.peek() == '\n' || stream.peek() == '\t')
		{
			stream.ignore();
			continue;
		}

		std::getline(stream, buffer);
		if (buffer.find("begin") == std::string::npos)
			break;

		Bone bone = {};
		while (stream)
		{
			std::string line;
			getline(stream, line);
			std::istringstream subStream{ line };

			std::string keyword;
			subStream >> keyword;
			if (0 == keyword.compare("id"))
			{
				subStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
			else if (0 == keyword.compare("name"))
			{
				subStream >> buffer;
				_boneNameList.push_back(buffer);
			}
			else if (0 == keyword.compare("direction"))
			{
				subStream >> bone.direction.x >> bone.direction.y >> bone.direction.z;
				// right handed coordinate to left handed coordinate
				bone.direction.z *= -1;
			}
			else if (0 == keyword.compare("length"))
			{
				subStream >> bone.length;
			}
			else if (0 == keyword.compare("axis"))
			{
				subStream >> bone.axis.x >> bone.axis.y >> bone.axis.z >> bone.axisOrder;
				// right handed coordinate to left handed coordinate
				bone.axis.x *= -1;
				bone.axis.y *= -1;
			}
			else if (0 == keyword.compare("dof"))
			{
				parseDOF(subStream, bone);
			}
			else if (0 == keyword.compare("end"))
				break;
		}
		_boneData.push_back(bone);
	}
}

void pa::ASF::parseHierarchy(std::ifstream& stream)
{
	_dfsRoute.reserve(_boneData.size());
	_boneParentList.resize(_boneData.size(), -1);

	{
		auto it = std::find(_boneNameList.begin(), _boneNameList.end(), "root");
		if (_boneNameList.end() == it)
			DebugBreak();

		const size_t rootBoneIndex = it - _boneNameList.begin();
		_dfsRoute.push_back(static_cast<int>(rootBoneIndex));
		_boneParentList[rootBoneIndex] = -1;
	}

	std::string buffer;
	stream >> buffer;
	stream.ignore();

	if ("begin" == buffer)
	{
		while (stream)
		{

			std::getline(stream, buffer);
			std::istringstream lineStream(buffer);

			lineStream >> buffer;
			lineStream.ignore();

			if (0 == buffer.compare("end"))
				break;

			const std::string& parentBoneName = buffer;
			auto it = std::find(_boneNameList.begin(), _boneNameList.end(), parentBoneName);
			if (_boneNameList.end() == it)
				DebugBreak();

			const size_t parentBoneIndex = it - _boneNameList.begin();

			std::string childBoneName;
			while (lineStream)
			{
				lineStream >> childBoneName;
				lineStream.ignore();

				if (childBoneName.empty())
					break;

				auto it = std::find(_boneNameList.begin(), _boneNameList.end(), childBoneName);
				if (_boneNameList.end() == it)
					DebugBreak();

				const size_t childBoneIndex = it - _boneNameList.begin();
				_dfsRoute.push_back(static_cast<int>(childBoneIndex));
				_boneParentList[childBoneIndex] = static_cast<int>(parentBoneIndex);
			}
		}
	}

}

void pa::ASF::parseDOF(std::istringstream& stream, Bone& bone)
{
	std::string buffer;
	int dofId = 0;
	while (stream)
	{
		stream >> buffer;
		stream.ignore();

		Bone::DOF dof = Bone::DOF::None;

		if (0 == buffer.compare("rx"))
			dof = Bone::DOF::RX;
		else if (0 == buffer.compare("ry"))
			dof = Bone::DOF::RY;
		else if (0 == buffer.compare("rz"))
			dof = Bone::DOF::RZ;
		else if (0 == buffer.compare("l"))
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

std::size_t pa::ASF::getBoneCount() const
{
	return _boneData.size();
}

DirectX::XMMATRIX pa::ASF::EulerRotation(const DirectX::XMFLOAT4& axis, const std::string& order)
{
	using namespace DirectX;
	// Use row major matrix
	const XMMATRIX rotationX = XMMatrixRotationX(axis.x * _unit.angle);
	const XMMATRIX rotationY = XMMatrixRotationY(axis.y * _unit.angle);
	const XMMATRIX rotationZ = XMMatrixRotationZ(axis.z * _unit.angle);

	XMMATRIX result = {};

	if (0 == order.compare("XYZ"))
		result = rotationX * rotationY * rotationZ;
	else if (0 == order.compare("XZY"))
		result = rotationX * rotationZ * rotationY;
	else if (0 == order.compare("YZX"))
		result = rotationY * rotationZ * rotationX;
	else if (0 == order.compare("YXZ"))
		result = rotationY * rotationX * rotationZ;
	else if (0 == order.compare("ZXY"))
		result = rotationZ * rotationX * rotationY;
	else if (0 == order.compare("ZYX"))
		result = rotationZ * rotationY * rotationX;

	return result;
}
