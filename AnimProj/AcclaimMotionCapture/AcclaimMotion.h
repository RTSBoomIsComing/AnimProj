// author: Changwan Yu
#pragma once
#include "Acclaim.h"
namespace pa
{
	class Acclaim::Motion
	{
	public:
		Motion(const Acclaim::Skeleton* skeleton, std::wstring const& filePath);
		~Motion() = default;

	private:
		void parseBoneNames(std::istream& stream);
		void parseMotionData(std::istream& stream);
		
	private:
		const Acclaim::Skeleton* const	_skeleton;
		std::vector<std::string>		_boneNames;
		std::vector<std::vector<float>>	_data;
		std::vector<uint16_t>			_boneIDs;
	};

}

