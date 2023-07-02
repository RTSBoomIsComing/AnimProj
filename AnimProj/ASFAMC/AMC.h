// author: Changwan Yu
#pragma once

namespace pa
{
	class ASF;
	class AMC
	{
	public:
		AMC() = default;
		AMC(const wchar_t* filePath);
		~AMC() = default;

		bool	loadFromFile(const wchar_t* filePath);
		//void	createMotion(const ASF* pASF, );
	private:
		std::vector<float>				_data;
		std::vector<std::string>		_dataOrder;
	};
}

