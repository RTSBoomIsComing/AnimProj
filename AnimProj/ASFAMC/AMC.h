// author: Changwan Yu
#pragma once

namespace pa
{
	class ASF;
	class Animation;
	class AMC
	{
		friend class RAnimation;
	public:
		AMC() = default;
		AMC(const wchar_t* filePath);
		~AMC() = default;
	
	public:
		bool							loadFromFile(const wchar_t* filePath);
		void							generateAnimation(ASF* pASF, Animation* pAnimation);

	public:
		inline float					getData(size_t index)	const	{ return _data[index]; }
		inline size_t					getFrameCount()			const	{ return _frameCount; }

	private:
		int								_frameCount;
		std::vector<float>				_data;
		std::vector<std::string>		_dataOrder;
	};
}

