// author: Changwan Yu
#include "pch.h"
#include "AcclaimMotion.h"
#include "AcclaimSkeleton.h"
pa::Acclaim::Motion::Motion(const Acclaim::Skeleton* skeleton, std::wstring const& filePath)
	: _skeleton(skeleton)
{
	assert(nullptr != _skeleton);

	std::ifstream stream(filePath);
	if (stream.fail())
		DebugBreak();

	parseBoneNames(stream);
	convertBoneNameToID();
	parseMotionData(stream);
}

void pa::Acclaim::Motion::parseBoneNames(std::istream& stream)
{
	std::string buffer;
	while (stream >> buffer)
	{
		if ('#' == buffer[0]
			|| ':' == buffer[0]
			|| std::isdigit(buffer[0]))
		{
			std::getline(stream, buffer);
			continue;
		}

		if ("root" == buffer && !_boneNames.empty())
			break;

		_boneNames.push_back(buffer);
		std::getline(stream, buffer);
	}
	stream.seekg(0);
}

void pa::Acclaim::Motion::parseMotionData(std::istream& stream)
{
	std::vector<Skeleton::Bone> const& boneData = _skeleton->_boneData;
	_data.resize(_boneNames.size());

	std::string buffer;
	while (stream >> buffer)
	{
		if ("root" != buffer)
			continue;

		_frameCount++;
		for (int i{}; i < boneData[0].dof.size(); i++)
		{
			float floatBuffer;
			stream >> floatBuffer;
			_data[0].push_back(floatBuffer);
		}

		for (int i = 1; i < _boneNames.size(); i++)
		{
			uint16_t boneID = _boneIDs[i];

			if (stream >> buffer && _boneNames[i] != buffer)
				DebugBreak();

			for (int j = 0; j < boneData[boneID].dof.size(); j++)
			{
				float floatBuffer;
				stream >> floatBuffer;
				_data[i].push_back(floatBuffer);
			}
		}
	}
}

void pa::Acclaim::Motion::convertBoneNameToID()
{
	std::vector<uint16_t> boneIDs;
	boneIDs.reserve(_boneNames.size());
	for (std::string const& name : _boneNames)
	{
		boneIDs.push_back(_skeleton->findBoneIDFromName(name));
	}
	_boneIDs = std::move(boneIDs);
}
