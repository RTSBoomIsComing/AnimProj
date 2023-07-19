// author: Changwan Yu
#include "pch.h"
#include "ASF.h"
#include "../Animation/Skeleton.h"

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

pa::ASF::ASF(const wchar_t* filePath)
{
	if (loadFromFile(filePath) != true)
		DebugBreak();
}

pa::ASF::ASF(const std::wstring& filePath)
{
	if (loadFromFile(filePath.c_str()) != true)
		DebugBreak();
}

bool pa::ASF::loadFromFile(const wchar_t* filePath)
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

pa::Skeleton* pa::ASF::createSkeleton()
{
	using namespace DirectX;

	Skeleton* skeleton = new Skeleton();
	skeleton->_boneHierarchy	= _boneHierarchy;
	skeleton->_parentList		= _parentList;
	
	for (size_t boneIndex = 0; boneIndex < getBoneCount(); boneIndex++)
	{
		
		XMMATRIX boneMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&_boneRotations[boneIndex]))* 
			XMMatrixTranslationFromVector(XMLoadFloat4(&_boneTranslations[boneIndex]));
		

		skeleton->_boneMatrices.emplace_back();
		XMStoreFloat4x4(&skeleton->_boneMatrices.back(), boneMatrix);
	}
	return skeleton;
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

	char	axisOrder[4]	= {};
	float	rootPosition[4]	= {};
	float	orientation[3]	= {};

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
			stream >> rootPosition[0] >> rootPosition[1] >> rootPosition[2];
			rootPosition[2] *= -1;
		}
		else if ("orientation" == buffer)
		{
			stream >> orientation[0] >> orientation[1] >> orientation[2];

			// right handed coordinate to left handed coordinate
			orientation[0] *= -1; 
			orientation[1] *= -1; 
		}
	}

	for (int i = 0; i < 3; i++)
	{
		orientation[i] *= _unitAngle;
		rootPosition[i] *= _unitLength;
	}


	_boneNameList.push_back("root");

	//Skeleton::Bone bone = {};
	const XMMATRIX rotation = eulerRotation(orientation, axisOrder);

	_boneRotations.emplace_back();
	XMStoreFloat4(&_boneRotations.back(), XMQuaternionNormalize(XMQuaternionRotationMatrix(rotation)));

	_boneTranslations.push_back(XMFLOAT4(rootPosition));
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
				continue;
			}
			else if ("name" == buffer)
			{
				line >> buffer;
				_boneNameList.push_back(buffer);
			}
			else if ("direction" == buffer)
			{
				line >> direction[0] >> direction[1] >> direction[2];

				// right handed coordinate to left handed coordinate
				direction[2] *= -1; 
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
				int channelCount = 0;
				while (line >> buffer)
				{
					dof.channels[channelCount++] = getChannel(buffer);
				}
			}
			else if ("end" == buffer)
				break;
		}

		for (int i = 0; i < 3; i++)
		{
			axis[i] *= _unitAngle;
		}

		_axisOrders.push_back(axisOrder);
		_DOFs.push_back(dof);


		XMFLOAT4 xmfloat4 = XMFLOAT4(direction);
		const XMVECTOR boneTranslation = XMLoadFloat4(&xmfloat4) * length * _unitLength;
		_boneTranslations.emplace_back();
		XMStoreFloat4(&_boneTranslations.back(), boneTranslation);

		const XMMATRIX rotation = eulerRotation(axis, axisOrder);
		_boneRotations.emplace_back();
		XMStoreFloat4(&_boneRotations.back(), XMQuaternionNormalize(XMQuaternionRotationMatrix(rotation)));
	}
}

void pa::ASF::parseHierarchy(std::istream& stream)
{
	_boneHierarchy.reserve(getBoneCount());
	_parentList.resize(getBoneCount(), std::numeric_limits<std::uint8_t>::max());

	const size_t rootBoneIndex = getBoneIndexFromName("root");
	_boneHierarchy.push_back(static_cast<uint8_t>(rootBoneIndex));

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
		const size_t parentIndex = getBoneIndexFromName(buffer);

		while (lineStream >> buffer)
		{
			// get child bone index
			const size_t childIndex = getBoneIndexFromName(buffer);

			_boneHierarchy.push_back(static_cast<uint8_t>(childIndex));
			_parentList[childIndex] = static_cast<uint8_t>(parentIndex);
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

	size_t boneCount = _boneRotations.size();

	// Convert Global space to Local Space 
	std::vector<XMFLOAT4> correctRotations(boneCount, XMFLOAT4{ 0.f, 0.f, 0.f, 1.f });
	std::vector<XMFLOAT4> correctTranslations(boneCount, XMFLOAT4{ 0.f, 0.f, 0.f, 1.f });
	for (size_t boneIndex = 1; boneIndex < boneCount; boneIndex++)
	{
		size_t parentIndex = _parentList[boneIndex];

		const XMVECTOR correctTranslation = XMVector3Transform(XMLoadFloat4(&_boneTranslations[boneIndex]), 
			XMMatrixRotationQuaternion(XMQuaternionInverse(XMLoadFloat4(&_boneRotations[boneIndex]))));
		XMStoreFloat4(&correctTranslations[boneIndex], correctTranslation);

		const XMVECTOR correctRotation = XMQuaternionMultiply(XMLoadFloat4(&_boneRotations[boneIndex]),
			XMQuaternionInverse(XMLoadFloat4(&_boneRotations[parentIndex])));
		XMStoreFloat4(&correctRotations[boneIndex], XMQuaternionNormalize(correctRotation));
	}

	_boneTranslations = correctTranslations;


	std::vector<uint8_t> newHierarchy;
	for (size_t boneIndex : _boneHierarchy)
	{
		_boneRotations[boneIndex] = XMFLOAT4{ 0.f, 0.f, 0.f, 1.f };

		newHierarchy.push_back(static_cast<uint8_t>(boneIndex));

		// find all children of this bone
		std::vector<size_t> childs;
		auto it = _parentList.begin();
		while ((it = std::find(it, _parentList.end(), static_cast<uint8_t>(boneIndex))) != _parentList.end())
		{
			childs.push_back(std::distance(_parentList.begin(), it));
			it++;
		}

		if (childs.size() == 1)
		{
			_boneRotations[boneIndex] = correctRotations[childs.front()];
			continue;
		}

		for (size_t childIndex : childs)
		{
			const size_t dummyBoneIndex = _boneRotations.size();
			newHierarchy.push_back(static_cast<uint8_t>(dummyBoneIndex));
			_parentList[childIndex] = static_cast<uint8_t>(dummyBoneIndex);
			_parentList.push_back(static_cast<uint8_t>(boneIndex));

			_boneRotations.push_back(correctRotations[childIndex]);
			_boneTranslations.emplace_back();

		}
	}

	_boneHierarchy = newHierarchy;
}

size_t pa::ASF::getBoneCount() const {
	assert(_boneRotations.size() == _boneTranslations.size());
	return _boneRotations.size(); 
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
