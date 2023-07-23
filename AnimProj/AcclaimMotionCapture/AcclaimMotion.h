// author: Changwan Yu
#pragma once
#include "Acclaim.h"
namespace pa
{
	class Acclaim::Motion
	{
		friend class AcclaimImporter;
	public:
		Motion(const Acclaim::Skeleton* skeleton, std::wstring const& filePath);
		~Motion() = default;

	private:
		void parseBoneNames(std::istream& stream);
		void parseMotionData(std::istream& stream);
		void convertBoneNameToID();
		
	private:
		const Acclaim::Skeleton* const		_skeleton;

		std::vector<std::string>			_boneNames;
		std::vector<uint16_t>				_boneIDs;
		std::vector<std::vector<float>>		_data;

		uint16_t							_frameCount = 0;
	};

}

