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

	std::vector<uint16_t> orderTable;
	std::vector<Skeleton::Bone> const& boneData = _skeleton->_boneData;
	for (std::string const& name : _boneNames)
	{
		const auto iter = std::find_if(boneData.begin(), boneData.end(),
			[&name](Skeleton::Bone const& bone)
			{
				return bone.name == name;
			});

		if (iter == boneData.end())
			DebugBreak();

		orderTable.push_back(std::distance(boneData.begin(), iter));
	}

	//size_t cursor = 0;
	//std::stringstream subStream(buffer);

	//subStream >> buffer;
	//if (!isCompleteParsingNames)
	//	_boneNames.push_back(buffer);

	//float floatBuffer;
	//while (subStream >> floatBuffer)
	//{
	//	_data.push_back(floatBuffer);
	//}



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
