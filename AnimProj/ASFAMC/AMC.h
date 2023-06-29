// author: Changwan Yu
#pragma once

namespace pa
{
	class AMC
	{
	public:
		AMC();
		AMC(const wchar_t* filePath);
		~AMC() = default;

		bool	loadFromFile(const wchar_t* filePath);

	private:
		
	};
}

