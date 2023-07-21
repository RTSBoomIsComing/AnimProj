// author: Changwan Yu
#include "pch.h"
#include "AcclaimSkeleton.h"

pa::Acclaim::Skeleton::Skeleton(std::wstring const& filePath)
{
	std::ifstream stream(filePath);
	if (stream.fail())
		DebugBreak();

	std::string buffer;
	while (stream >> buffer)
	{
		if ('#' == buffer[0])
		{
			std::getline(stream, buffer);
			continue;
		}

		if (":units" == buffer)
		{
			if (stream >> buffer && "mass" != buffer)
				DebugBreak();

			stream >> _unit.mass;

			if (stream >> buffer && "length" != buffer)
				DebugBreak();
	
			stream >> _unit.length;

			if (stream >> buffer && "angle" != buffer)
				DebugBreak();

			if (stream >> buffer && "deg" == buffer)
				_unit.angle = DirectX::XM_PI / 180.0f;
			
			continue;
		}
		if (":root" == buffer)
			break;
	}

	{	
		if (stream >> buffer && "order" != buffer)
			DebugBreak();

		for (int i = 0; i < 6; i++)
		{
			stream >> buffer;
			_root.order.push_back(Acclaim::convertStringToDOF(buffer));
		}

		if (stream >> buffer && "axis" != buffer)
			DebugBreak();

		stream >> buffer;
		_root.axis = Acclaim::convertStringToAxisOrder(buffer);

		if (stream >> buffer && "position" != buffer)
			DebugBreak();

		stream >> _root.position.x >> _root.position.y >> _root.position.z;

		if (stream >> buffer && "orientation" != buffer)
			DebugBreak();

		stream >> _root.orientation.x >> _root.orientation.y >> _root.orientation.z;
	}

	if (stream >> buffer && ":bonedata" != buffer)
		DebugBreak();

	_boneData.push_back(this->convertRootToBone(_root));

	while (stream >> buffer && "begin" == buffer)
	{
		Bone bone = {};
		while (stream >> buffer && "name" != buffer);
		stream >> bone.name;

		if (stream >> buffer && "direction" != buffer)
			DebugBreak();

		stream >> bone.direction.x >> bone.direction.y >> bone.direction.z;
				
		if (stream >> buffer && "length" != buffer)
			DebugBreak();

		stream >> bone.length;

		if (stream >> buffer && "axis" != buffer)
			DebugBreak();

		stream >> bone.axis.x >> bone.axis.y >> bone.axis.z >> buffer;
		_root.axis = Acclaim::convertStringToAxisOrder(buffer);

		stream >> buffer;
		if ("dof" == buffer)
		{
			while (stream >> buffer && "limits" != buffer)
			{
				bone.dof.push_back(Acclaim::convertStringToDOF(buffer));
			}
		}

		_boneData.push_back(bone);
		while ("end" != buffer)
		{
			stream >> buffer;
		}
	}

	if (":hierarchy" != buffer)
		DebugBreak();

	stream >> buffer;
	if ("begin" != buffer)
		DebugBreak();

	std::vector<bool> isVisit(_boneData.size());
	isVisit[0] = true;

	_parents.resize(_boneData.size());

	_hierarchy.reserve(_boneData.size());
	_hierarchy.push_back(0);

	uint16_t parentID = 0;
	while (stream >> buffer && "end" != buffer)
	{
		uint16_t boneID = findBoneIDFromName(buffer);
		if (isVisit[boneID])
		{
			parentID = boneID;
			continue;
		}
		
		_hierarchy.push_back(boneID);
		_parents[boneID] = parentID;

		isVisit[boneID] = true;
	}
}

uint16_t pa::Acclaim::Skeleton::findBoneIDFromName(std::string const& name) const
{
	const auto iter = std::find_if(_boneData.begin(), _boneData.end(),
		[&name](Skeleton::Bone const& bone)
		{
			return bone.name == name;
		});

	if (iter == _boneData.end())
		DebugBreak();

	return std::distance(_boneData.begin(), iter);
}

uint16_t pa::Acclaim::Skeleton::getParentID(uint16_t boneID) const
{
	return _parents[boneID];
}

DirectX::XMVECTOR pa::Acclaim::Skeleton::getBoneTranslation(uint16_t boneID) const
{
	using namespace DirectX;
	if (0 == boneID)
	{
		XMVECTOR V = XMLoadFloat3(&_root.position);
		V = V * _unit.length;

		return V;
	}

	const Bone& bone = _boneData[boneID];

	XMVECTOR V = XMLoadFloat3(&bone.direction);
	V = V * _unit.length;

	XMVECTOR axis = XMLoadFloat3(&bone.axis);
	axis = axis * _unit.angle;

	const XMVECTOR Q = Acclaim::getQuaternionFromAxis(axis, bone.axisOrder);
	const XMVECTOR Qinversed = XMQuaternionConjugate(Q);
	V = XMVector3Rotate(V, Qinversed);

	return V;
}

DirectX::XMVECTOR pa::Acclaim::Skeleton::getBoneRotation(uint16_t boneID) const
{
	using namespace DirectX;
	Skeleton::Bone const& bone = _boneData[boneID];
	Skeleton::Bone const& parent = _boneData[this->getParentID(boneID)];

	XMVECTOR axis = XMLoadFloat3(&bone.axis);
	XMVECTOR parentAxis = XMLoadFloat3(&parent.axis);

	axis		= axis * _unit.angle;
	parentAxis	= parentAxis * _unit.angle;

	XMVECTOR		Q		= Acclaim::getQuaternionFromAxis(axis, bone.axisOrder);
	const XMVECTOR	Qparent	= Acclaim::getQuaternionFromAxis(parentAxis, parent.axisOrder);
		
	return XMQuaternionMultiply(Q, Qparent);;
}

pa::Acclaim::Skeleton::Bone pa::Acclaim::Skeleton::convertRootToBone(const Root& root)
{
	Bone bone = {};
	bone.name = "root";
	bone.axis = root.orientation;
	bone.axisOrder = root.axis;
	bone.dof = root.order;

	return bone;
}
