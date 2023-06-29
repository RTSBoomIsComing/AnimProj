// author: Changwan Yu
#include "pch.h"
#include "ASF.h"

pa::ASF::ASF(const wchar_t* filePath)
{
	if (false == loadFromFile(filePath))
		DebugBreak();

	using namespace DirectX;


	std::unordered_map<std::string, XMMATRIX> parentTransforms;
	parentTransforms["root"] = XMMatrixIdentity();

	_dfsBoneTransforms.resize(_dfsRoute.size());
	XMMATRIX rootRotation = EulerRotation(_boneData["root"].axis, _boneData["root"].axisOrder);
	XMMATRIX rootRotationInverse = XMMatrixInverse(nullptr, rootRotation);
	{
		XMVECTOR vectorScale{ _unit.length, _unit.length, _unit.length, 0.0f };
		XMMATRIX translation =
			XMMatrixTranslationFromVector(
				XMLoadFloat4(&_rootPosition) * vectorScale);

		XMStoreFloat4x4(&_dfsBoneTransforms[0], rootRotation * translation);
	}

	for (int i = 1; i < _dfsRoute.size(); i++)
	{
		const std::string& boneName = _dfsRoute[i];
		Bone& bone = _boneData[boneName];

		const std::string& parentName = _boneParentMap[boneName];
		const XMMATRIX& parentTransform = parentTransforms[parentName];

		const float vectorScaleFactor = _unit.length * bone.length;
		XMVECTOR vectorScale{ vectorScaleFactor, vectorScaleFactor, vectorScaleFactor, 0.0f };

		XMMATRIX translation = XMMatrixTranslationFromVector(
			XMLoadFloat4(&bone.direction) * vectorScale);

		XMMATRIX rotation = EulerRotation(bone.axis, bone.axisOrder);

		XMMATRIX boneLocalTransform = rootRotationInverse * rotation * rootRotation * translation;
		XMMATRIX boneGobalTransfrom = boneLocalTransform * parentTransform;
		parentTransforms[boneName] = boneGobalTransfrom;
		XMStoreFloat4x4(&_dfsBoneTransforms[i], boneGobalTransfrom);
	}
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

		else if (0 == line.compare(":root"))
			parseRoot(stream);

		else if (0 == line.compare(":bonedata"))
			parseBoneData(stream);

		else if (0 == line.compare(":hierarchy"))
			parseHierarchy(stream);
	}

	return true;
}

void pa::ASF::parseUnits(std::ifstream& stream)
{
	for (int i = 0; i < 3; i++)
	{
		std::string unitName;
		stream >> unitName;
		if (0 == unitName.compare("mass"))
			stream >> _unit.mass;

		if (0 == unitName.compare("length"))
			stream >> _unit.length;

		if (0 == unitName.compare("angle"))
		{
			std::string unitAngle;
			stream >> unitAngle;
			_unit.angle = (0 == unitAngle.compare("deg")) ? (DirectX::XM_PI / 180) : 1.0f;
		}
	}
}

void pa::ASF::parseRoot(std::ifstream& stream)
{
	Bone bone = {};
	bone.name = "root";
	
	while (stream && stream.peek() != static_cast<int>(':'))
	{
		std::string line;
		std::getline(stream, line);
		std::istringstream subStream{ line };
		
		std::string buffer;
		subStream >> buffer;

		if (0 == buffer.compare("order"))
			parseAMCRootDataOrder(subStream);
	
		else if (0 == buffer.compare("axis"))
			subStream >> bone.axisOrder;
	
		else if (0 == buffer.compare("position"))
			subStream >> _rootPosition.x >> _rootPosition.y >> _rootPosition.z;

		else if (0 == buffer.compare("orientation"))
			subStream >> bone.axis.x >> bone.axis.y >> bone.axis.z;
	
	}

	_boneData["root"] = bone;
}

void pa::ASF::parseBoneData(std::ifstream& stream)
{
	
	while (stream && stream.peek() != static_cast<int>(':'))
	{
		std::string buffer;
		stream >> buffer;
		if (0 != buffer.compare("begin"))
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
				subStream >> bone.id;

			else if (0 == keyword.compare("name"))
				subStream >> bone.name;

			else if (0 == keyword.compare("direction"))
				subStream >> bone.direction.x >> bone.direction.y >> bone.direction.z;

			else if (0 == keyword.compare("length"))
				subStream >> bone.length;

			else if (0 == keyword.compare("axis"))
				subStream >> bone.axis.x >> bone.axis.y >> bone.axis.z >> bone.axisOrder;

			else if (0 == keyword.compare("dof"))
				parseDOF(subStream, bone);

			else if (0 == keyword.compare("end"))
				break;
		}

		_boneData[bone.name] = bone;
	}
}

void pa::ASF::parseHierarchy(std::ifstream& stream)
{
	_dfsRoute.reserve(_boneData.size());
	_dfsRoute.emplace_back("root");

	std::string buffer;
	stream >> buffer;
	if (0 == buffer.compare("begin"))
	{
		while (stream)
		{
			std::string lineBuffer;
			std::getline(stream, lineBuffer);
			std::stringstream lineStream{ lineBuffer };

			lineStream >> buffer;
			if (0 == buffer.compare("end"))
				break;

			std::string parentBoneName = buffer;
			while (lineStream)
			{
				std::string childBoneName;
				lineStream >> childBoneName;
				if (childBoneName.empty())
					break;
				_boneParentMap[childBoneName] = parentBoneName;
				_dfsRoute.push_back(childBoneName);
			}
		}
	}

}

void pa::ASF::parseAMCRootDataOrder(std::istringstream& stream)
{
	
	for (int i = 0; i < 6; i++)
	{
		std::string buffer;
		stream >> buffer;
		if (0 == buffer.compare("TX"))
		{
			_rootOrder[0] = Bone::DOF::TX;
			continue;
		}

		if (0 == buffer.compare("TY"))
		{
			_rootOrder[1] = Bone::DOF::TY;
			continue;
		}

		if (0 == buffer.compare("TZ"))
		{
			_rootOrder[2] = Bone::DOF::TZ;
			continue;
		}

		if (0 == buffer.compare("RX"))
		{
			_rootOrder[3] = Bone::DOF::RX;
			continue;
		}

		if (0 == buffer.compare("RY"))
		{
			_rootOrder[4] = Bone::DOF::RY;
			continue;
		}

		if (0 == buffer.compare("RZ"))
		{
			_rootOrder[5] = Bone::DOF::RZ;
			continue;
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

const std::vector<DirectX::XMFLOAT4X4> pa::ASF::getGlobalBoneTransforms() const
{
	return _dfsBoneTransforms;
}

DirectX::XMMATRIX pa::ASF::EulerRotation(const DirectX::XMFLOAT4& axis, const std::string& order)
{
	using namespace DirectX;
	// Use row major matrix
	const XMMATRIX rotationX = XMMatrixRotationX(axis.x * _unit.angle);
	const XMMATRIX rotationY = XMMatrixRotationX(axis.y * _unit.angle);
	const XMMATRIX rotationZ = XMMatrixRotationX(axis.z * _unit.angle);

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
