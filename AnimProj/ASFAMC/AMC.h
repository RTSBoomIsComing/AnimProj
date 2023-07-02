// author: Changwan Yu
#pragma once

namespace pa
{
	class AMC
	{
	public:
		AMC() = default;
		AMC(const wchar_t* filePath);
		~AMC() = default;

		bool	loadFromFile(const wchar_t* filePath);
	private:
		std::vector<float>				_data;
		std::vector<std::string>		_dataOrder;
	};
}

