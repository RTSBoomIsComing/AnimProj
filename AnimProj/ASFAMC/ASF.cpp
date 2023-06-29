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
	XMMATRIX rootRotation = EulerRotation(_boneData[0].axis, _boneData[0].axisOrder);
	XMMATRIX rootRotationInverse = XMMatrixInverse(nullptr, rootRotation);
	{
		XMMATRIX translation =
			XMMatrixTranslationFromVector(
				XMLoadFloat4(&_rootPosition) * XMVECTOR{ _unitLength, _unitLength, _unitLength, 0.0f });

		
		XMStoreFloat4x4(&_dfsBoneTransforms[0], rootRotation* translation);
	}

	for (int i = 1; i < _dfsRoute.size(); i++)
	{
		const std::string& boneName = _dfsRoute[i];
		const int boneIndex = _boneNameMap[boneName];
		Bone& bone = _boneData[boneIndex];

		const std::string& parentName = _boneParentArray[boneIndex];
		const XMMATRIX& parentTransform = parentTransforms[parentName];

		const float translationScale = _unitLength * bone.length;
		XMMATRIX translation = XMMatrixTranslationFromVector(
			XMLoadFloat4(&bone.direction) * XMVECTOR{ translationScale, translationScale, translationScale, 0.0f }
		);

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
	_boneData.back().name = "root";
	_boneNameMap["root"] = 0;

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
		auto rollBackPosition = stream.tellg();
		stream >> buffer;
		if (0 == buffer.compare("begin"))
		{
			int boneIndex = static_cast<int>(_boneData.size());
			_boneData.emplace_back();
			parseEachBone(stream, boneIndex);
		}
		else
		{
			stream.seekg(rollBackPosition);
			stream.seekg(-1, std::ios::cur);
			break;
		}
	}

}

void pa::ASF::parseEachBone(std::ifstream& stream, int boneIndex)
{
	std::cout << "\t\tPars Each Bone\n";
	Bone& bone = _boneData[boneIndex];

	std::string buffer;
	while (stream)
	{
		stream >> buffer;

		if (buffer.find("name") != std::string::npos)
		{
			stream >> bone.name;
			_boneNameMap[bone.name] = boneIndex;
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
	// TODO
	_boneParentArray.resize(_boneData.size());
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
				_boneParentArray[_boneNameMap[childBoneName]] = parentBoneName;
				_dfsRoute.push_back(childBoneName);
			}
		}
	}

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
	float position[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	for (int i = 0; i < 3; i++)
	{
		stream >> position[i];
	}
	_rootPosition = DirectX::XMFLOAT4{ position };
}

void pa::ASF::parseAxis(std::ifstream& stream, Bone& bone)
{
	float axis[4] = { 0.0f, 0.0f, 0.0f, 0.0f/*not used*/};
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

const std::vector<DirectX::XMFLOAT4X4> pa::ASF::getGlobalBoneTransforms() const
{
	return _dfsBoneTransforms;
}

DirectX::XMMATRIX pa::ASF::EulerRotation(const DirectX::XMFLOAT4& axis, Bone::AxisOrder order)
{
	using namespace DirectX;
	// Use row major matrix
	const float angleScale = (_unitAngle == UnitAngle::Degree) ? (DirectX::XM_PI / 180) : 1.0f;
	const XMMATRIX rotationX = XMMatrixRotationX(axis.x * angleScale);
	const XMMATRIX rotationY = XMMatrixRotationX(axis.y * angleScale);
	const XMMATRIX rotationZ = XMMatrixRotationX(axis.z * angleScale);

	XMMATRIX result = {};
	switch (order)
	{
	case Bone::AxisOrder::XYZ:
		result = rotationX * rotationY * rotationZ;
		break;
	case Bone::AxisOrder::XZY:
		result = rotationX * rotationZ * rotationY;
		break;
	case Bone::AxisOrder::YZX:
		result = rotationY * rotationZ * rotationX;
		break;
	case Bone::AxisOrder::YXZ:
		result = rotationY * rotationX * rotationZ;
		break;
	case Bone::AxisOrder::ZXY:
		result = rotationZ * rotationX * rotationY;
		break;
	case Bone::AxisOrder::ZYX:
		result = rotationZ * rotationY * rotationX;
		break;
	}

	return result;
}
