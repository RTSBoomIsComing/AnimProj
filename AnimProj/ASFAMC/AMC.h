// author: Changwan Yu
#pragma once

namespace pa
{
	class ASF;
	class Animation;
	class AMC
	{
	public:
		AMC() = default;
		AMC(const wchar_t* filePath);
		~AMC() = default;

		bool							loadFromFile(const wchar_t* filePath);
		void							generateAnimation(const ASF* pASF, Animation* pAnimation);
	private:
		int								_frameCount;
		std::vector<float>				_data;
		std::vector<std::string>		_dataOrder;
	};
}

