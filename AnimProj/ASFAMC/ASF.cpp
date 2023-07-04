// author: Changwan Yu
#include "pch.h"
#include "ASF.h"
#include "../Rendering/Skeleton.h"

static std::istream& ignoreEmptyChar(std::istream& stream)
{
	while (stream)
	{

		if (stream.peek() == ' ' || stream.peek() == '\n' || stream.peek() == '\t')
			stream.ignore();
		else
			break;
	}

	return stream;
}

pa::ASF::ASF(pa::Skeleton* pSkeleton)
	: _pSkeleton(pSkeleton)
{}

pa::ASF::ASF(const wchar_t* filePath, pa::Skeleton* pSkeleton)
	: _pSkeleton(pSkeleton)
{
	if (loadFromFile(filePath, pSkeleton) != true)
		DebugBreak();
}

bool pa::ASF::loadFromFile(const wchar_t* filePath, pa::Skeleton* pSkeleton)
{
	std::ifstream stream{ filePath };
	if (stream.fail())
		return false;

	std::string line;
	while (std::getline(stream, line))
	{
		if (":units" == line)
		{
			parseUnits(stream);
		}
		else if (":root" == line)
		{
			parseRoot(stream);
		}
		else if (":bonedata" == line)
		{
			parseBoneData(stream);
		}
		else if (":hierarchy" == line)
		{
			parseHierarchy(stream);
		}
	}

	return true;
}

void pa::ASF::parseUnits(std::istream& stream)
{
	std::string line;
	for (int i = 0; i < 3; i++)
	{
		std::getline(stream, line);

		std::size_t findPos;
		findPos = line.find("mass");
		if (findPos != std::string::npos)
		{
			_unitMass = std::stof(line.substr(findPos + sizeof("mass")));
			continue;
		}

		findPos = line.find("length");
		if (findPos != std::string::npos)
		{
			_unitLength = std::stof(line.substr(findPos + sizeof("length")));
			continue;
		}


		findPos = line.find("angle");
		if (findPos != std::string::npos)
		{
			_unitAngle = ("deg" == line.substr(findPos + sizeof("angle"))) ? (DirectX::XM_PI / 180) : 1.0f;
			continue;
		}
	}
}

void pa::ASF::parseRoot(std::istream& stream)
{
	using namespace DirectX;

	char axisOrder[4];
	float orientation[3] = {};

	std::string buffer;
	for (int i = 0; i < 4; i++)
	{
		stream >> buffer;
		if ("order" == buffer)
		{
			std::string order;
			Channel dof[6] = {};
			for (int j = 0; j < 6; j++)
			{
				stream >> order;
				dof[j] = getChannel(order);
			}
			_DOF.push_back(dof);
		}
		else if ("axis" == buffer)
		{
			stream >> axisOrder;
			_axisOrder.push_back(axisOrder);
		}
		else if ("position" == buffer)
		{
			stream >> _rootPosition.x >> _rootPosition.y >> _rootPosition.z;
		}
		else if ("orientation" == buffer)
		{
			stream >> orientation[0] >> orientation[1] >> orientation[2];
		}
	}
	_boneNameList.push_back("root");

	Skeleton::Bone bone = {};
	const XMMATRIX rotation = EulerRotation(orientation, axisOrder);
	XMStoreFloat4(&bone.rotation, XMQuaternionRotationMatrix(rotation));

	_pSkeleton->_boneList.push_back(bone);
}

void pa::ASF::parseBoneData(std::istream& stream)
{
	float length = 1.0f;
	float axis[3] = {};
	char  axisOrder[4];

	std::string buffer;
	while (ignoreEmptyChar(stream) && stream.peek() != ':')
	{
		std::getline(stream, buffer);
		if (buffer.find("begin") == std::string::npos)
			break;

		Skeleton::Bone bone = {};
		while (ignoreEmptyChar(stream))
		{
			getline(stream, buffer);
			std::istringstream line{ buffer };

			line >> buffer;
			if ("id" == buffer)
			{
				line.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
			else if ("name" == buffer)
			{
				line >> buffer;
				_boneNameList.push_back(buffer);
			}
			else if ("direction" == buffer)
			{
				line >> bone.direction.x >> bone.direction.y >> bone.direction.z;
				// right handed coordinate to left handed coordinate
				bone.direction.z *= -1;
			}
			else if ("length" == buffer)
			{
				line >> length;
			}
			else if ("axis" == buffer)
			{
				line >> axis[0] >> axis[1] >> axis[2] >> axisOrder;
				// right handed coordinate to left handed coordinate
				axis[0] *= -1;
				axis[1] *= -1;
			}
			else if ("dof" == buffer)
			{
				while (line >> buffer);
				
				// TODO
				// need to get dof
			}
			else if ("end" == buffer)
				break;
		}
		using namespace DirectX;
		const XMVECTOR adjustedDirection = XMLoadFloat4(&bone.direction) * length * _unitLength;
		XMStoreFloat4(&bone.direction, adjustedDirection);

		const XMMATRIX rotation = EulerRotation(axis, axisOrder);
		const XMVECTOR quaternion = XMQuaternionRotationMatrix(rotation);
		XMStoreFloat4(&bone.rotation, quaternion);

		_pSkeleton->_boneList.push_back(bone);
	}
}

void pa::ASF::parseHierarchy(std::istream& stream)
{
	std::size_t boneCount = this->getBoneCount();

	_pSkeleton->_DFSPath.reserve(boneCount);
	_pSkeleton->_parentList.resize(boneCount, std::numeric_limits<std::uint8_t>::max());

	const size_t rootBoneIndex = getBoneIndexFromName("root");
	_pSkeleton->_DFSPath.push_back(static_cast<int>(rootBoneIndex));

	// already initialized with max of uint8_t 
	// _pSkeleton->_parentList[rootBoneIndex] = std::numeric_limits<std::uint8_t>::max();

	std::string buffer;
	stream >> buffer;
	if ("begin" != buffer)
	{
		DebugBreak();
		return;
	}

	while (stream)
	{
		ignoreEmptyChar(stream);
		std::getline(stream, buffer);
		std::istringstream lineStream(buffer);

		lineStream >> buffer;
		if (0 == buffer.compare("end"))
			break;

		const size_t parentBoneIndex = getBoneIndexFromName(buffer);

		while (lineStream)
		{
			lineStream >> buffer;
			const size_t childBoneIndex = getBoneIndexFromName(buffer);

			_pSkeleton->_DFSPath.push_back(static_cast<uint8_t>(childBoneIndex));
			_pSkeleton->_parentList[childBoneIndex] = static_cast<uint8_t>(parentBoneIndex);
		}
	}


}

pa::ASF::Channel pa::ASF::getChannel(std::string channelName) const
{
	std::string channelNames = "rxryrztxtytzl";

	std::transform(channelName.begin(), channelName.end(), channelName.begin(), ::tolower);

	std::size_t findPos = channelNames.find(channelName);
	if (findPos != std::string::npos)
		return static_cast<Channel>(findPos / 2);

	return Channel::None;
}

std::size_t pa::ASF::getBoneIndexFromName(const std::string& boneName) const
{
	auto it = std::find(_boneNameList.begin(), _boneNameList.end(), boneName);
	if (_boneNameList.end() == it)
		return std::numeric_limits<std::size_t>::max();

	return it - _boneNameList.begin();
}



std::size_t pa::ASF::getBoneCount() const
{
	return _boneNameList.size();
}

DirectX::XMMATRIX pa::ASF::EulerRotation(const float axis[3], const std::string& order)
{
	using namespace DirectX;
	// Use row major matrix
	const XMMATRIX rotationX = XMMatrixRotationX(axis[0] * _unitAngle);
	const XMMATRIX rotationY = XMMatrixRotationY(axis[1] * _unitAngle);
	const XMMATRIX rotationZ = XMMatrixRotationZ(axis[2] * _unitAngle);

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
