// author: Changwan Yu
#pragma once
#include "Acclaim.h"
namespace pa
{
	class Acclaim::Motion
	{
	public:
		Motion(std::wstring const& filePath);
		~Motion() = default;

	private:
		std::vector<std::string>	_boneNames;
		std::vector<float>			_data;
	};

}

