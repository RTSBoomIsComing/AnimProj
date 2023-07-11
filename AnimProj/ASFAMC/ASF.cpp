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

pa::ASF::ASF(pa::Skeleton* pSkeleton, const wchar_t* filePath)
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

	correctSkeleton();

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

	char axisOrder[4] = {};
	float orientation[3] = {};

	std::string buffer;
	for (int i = 0; i < 4; i++)
	{
		stream >> buffer;
		if ("order" == buffer)
		{
			std::string order;
			DOF dof = {};
			for (int j = 0; j < 6; j++)
			{
				stream >> order;
				dof.channels[j] = getChannel(order);
			}
			_DOFs.push_back(dof);
		}
		else if ("axis" == buffer)
		{
			stream >> axisOrder;
			_axisOrders.push_back(axisOrder);
		}
		else if ("position" == buffer)
		{
			stream >> _rootPosition.x >> _rootPosition.y >> _rootPosition.z;
		}
		else if ("orientation" == buffer)
		{
			stream >> orientation[0] >> orientation[1] >> orientation[2];
			orientation[0] *= _unitAngle * -1; // right handed coordinate to left handed coordinate
			orientation[1] *= _unitAngle * -1; // right handed coordinate to left handed coordinate
			orientation[2] *= _unitAngle;
		}
	}
	_boneNameList.push_back("root");

	Skeleton::Bone bone = {};
	const XMMATRIX rotation = eulerRotation(orientation, axisOrder);
	XMStoreFloat4(&bone.rotation, XMQuaternionNormalize(XMQuaternionRotationMatrix(rotation)));

	_pSkeleton->_boneList.push_back(bone);
}

void pa::ASF::parseBoneData(std::istream& stream)
{
	using namespace DirectX;

	std::string buffer;
	while (ignoreEmptyChar(stream) && stream.peek() != ':')
	{
		std::getline(stream, buffer);
		if (buffer.find("begin") == std::string::npos)
			break;

		float	length = 1.0f;
		float	axis[3] = {};
		char	axisOrder[4] = {};
		float	direction[4] = {};
		DOF		dof = {};
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
				line >> direction[0] >> direction[1] >> direction[2];
				direction[2] *= -1; // right handed coordinate to left handed coordinate
			}
			else if ("length" == buffer)
			{
				line >> length;
			}
			else if ("axis" == buffer)
			{
				line >> axis[0] >> axis[1] >> axis[2] >> axisOrder;

				axis[0] *= _unitAngle * -1; // right handed coordinate to left handed coordinate
				axis[1] *= _unitAngle * -1; // right handed coordinate to left handed coordinate
				axis[2] *= _unitAngle;
			}
			else if ("dof" == buffer)
			{
				int channelCount = 0;
				while (line >> buffer)
				{
					dof.channels[channelCount++] = getChannel(buffer);
				}
			}
			else if ("end" == buffer)
				break;
		}

		_axisOrders.push_back(axisOrder);
		_DOFs.push_back(dof);

		Skeleton::Bone bone = {};

		bone.direction = XMFLOAT4(direction);
		const XMVECTOR adjustedDirection = XMLoadFloat4(&bone.direction) * length * _unitLength;
		XMStoreFloat4(&bone.direction, adjustedDirection);

		const XMMATRIX rotation = eulerRotation(axis, axisOrder);
		const XMVECTOR quaternion = XMQuaternionNormalize(XMQuaternionRotationMatrix(rotation));
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

	std::string buffer;
	stream >> buffer;
	if ("begin" != buffer)
	{
		DebugBreak();
		return;
	}

	while (ignoreEmptyChar(stream) && std::getline(stream, buffer))
	{
		std::istringstream lineStream(buffer);
		lineStream >> buffer;

		if ("end" == buffer)
			break;

		// get parent bone index
		const size_t parentBoneIndex = getBoneIndexFromName(buffer);

		while (lineStream >> buffer)
		{
			// get child bone index
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

void pa::ASF::correctSkeleton()
{
	using namespace DirectX;

	// Convert Global space to Local Space 
	std::vector<XMFLOAT4> convertedQuats(_pSkeleton->getBoneCount(), XMFLOAT4{ 0.f, 0.f, 0.f, 1.f });
	for (size_t i = 1; i < _pSkeleton->getBoneCount(); i++)
	{
		const Skeleton::Bone& bone = _pSkeleton->getBone(i);
		const Skeleton::Bone& parentBone = _pSkeleton->getBone(_pSkeleton->getParentBoneIndex(i));

		XMVECTOR dir = XMVector3Transform(XMLoadFloat4(&bone.direction), XMMatrixRotationQuaternion(
			XMQuaternionInverse(XMLoadFloat4(&bone.rotation))));
		XMStoreFloat4(&_pSkeleton->_boneList[i].direction, dir);

		XMVECTOR quat = XMQuaternionMultiply(XMLoadFloat4(&bone.rotation),
			XMQuaternionInverse(XMLoadFloat4(&parentBone.rotation)));

		XMStoreFloat4(&convertedQuats[i], quat);
	}

	for (size_t i = 1; i < _pSkeleton->getBoneCount(); i++)
	{
		_pSkeleton->_boneList[i].rotation = convertedQuats[i];
	}

	std::vector<uint8_t> hierarchy;
	for (size_t currentBonIndex : _pSkeleton->getDFSPath())
	{
		_pSkeleton->_boneList[currentBonIndex].rotation = XMFLOAT4{ 0.f, 0.f, 0.f, 1.f };

		hierarchy.push_back(static_cast<uint8_t>(currentBonIndex));

		// find all children of this bone
		std::vector<size_t> childs;
		auto it = _pSkeleton->_parentList.begin();
		while ((it = std::find(it, _pSkeleton->_parentList.end(), static_cast<uint8_t>(currentBonIndex))) != _pSkeleton->_parentList.end())
		{
			childs.push_back(std::distance(_pSkeleton->_parentList.begin(), it));
			it++;
		}

		if (childs.size() == 1)
		{
			_pSkeleton->_boneList[currentBonIndex].rotation = _pSkeleton->getBone(childs.front()).rotation;
			continue;
		}

		for (size_t childBoneIndex : childs)
		{
			Skeleton::Bone dummyBone;
			dummyBone.rotation = _pSkeleton->getBone(childBoneIndex).rotation;

			const size_t dummyBoneIndex = _pSkeleton->getBoneCount();

			_pSkeleton->_boneList.push_back(dummyBone);
			_pSkeleton->_parentList.push_back(static_cast<uint8_t>(currentBonIndex));
			hierarchy.push_back(static_cast<uint8_t>(dummyBoneIndex));

			_pSkeleton->_parentList[childBoneIndex] = static_cast<uint8_t>(dummyBoneIndex);
		}
	}

	_pSkeleton->_DFSPath = hierarchy;


}

DirectX::XMMATRIX pa::ASF::eulerRotation(const float axis[3], const std::string& order)
{
	using namespace DirectX;
	// Use row major matrix
	const XMMATRIX rotationX = XMMatrixRotationX(axis[0]);
	const XMMATRIX rotationY = XMMatrixRotationY(axis[1]);
	const XMMATRIX rotationZ = XMMatrixRotationZ(axis[2]);

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
	else
		result = rotationX * rotationY * rotationZ;

	return result;
}
