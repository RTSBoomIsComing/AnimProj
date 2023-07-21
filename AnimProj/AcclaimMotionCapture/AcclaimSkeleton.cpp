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
			stream >> _unit.mass >> _unit.length >> buffer;
			if ("deg" == buffer)
			{
				constexpr float radian = 0.0174533;
				_unit.angle = radian;
			}
			continue;
		}

		if (":root" == buffer)
		{
			stream >> buffer;
			if ("order" != buffer)
				DebugBreak();

			for (int i = 0; i < 6; i++)
			{
				stream >> buffer;
				_root.order.push_back(Acclaim::convertStringToDOF(buffer));
			}

			stream >> buffer;
			if ("axis" != buffer)
				DebugBreak();

			stream >> buffer;
			_root.axis = Acclaim::convertStringToAxisOrder(buffer);

			stream >> buffer;
			if ("position" != buffer)
				DebugBreak();

			stream >> _root.position.x >> _root.position.y >> _root.position.z;

			stream >> buffer;
			if ("orientation" != buffer)
				DebugBreak();

			stream >> _root.orientation.x >> _root.orientation.y >> _root.orientation.z;
			
			continue;
		}




		_boneNames.push_back(buffer);
		std::getline(stream, buffer);
	}
}
