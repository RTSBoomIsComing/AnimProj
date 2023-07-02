// author: Changwan Yu
#pragma once

namespace pa
{
	struct AnimationSheet
	{
		std::vector<DirectX::XMMATRIX> rotations;
	};

	class ASF;
	class AMC
	{
	public:
		AMC() = default;
		AMC(const wchar_t* filePath);
		~AMC() = default;

		bool							loadFromFile(const wchar_t* filePath);
		void							generateAnimation(const ASF* pASF);
	private:
		int								_frameCount;
		std::vector<float>				_data;
		std::vector<std::string>		_dataOrder;
	public:

		// temporarily open member to public
		std::vector<AnimationSheet>		_animationSheets;
		
	};
}

