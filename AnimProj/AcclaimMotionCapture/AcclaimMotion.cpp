// author: Changwan Yu
#include "pch.h"
#include "AcclaimMotion.h"

pa::Acclaim::Motion::Motion(const Acclaim::Skeleton* skeleton, std::wstring const& filePath)
	: _skeleton(skeleton)
{
	std::ifstream stream(filePath);
	if (stream.fail())
		DebugBreak();

	parseBoneNames(stream);
	
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
