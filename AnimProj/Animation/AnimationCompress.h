#pragma once
#include "CompressedFrame.h"

namespace pa
{
	class Animation;
	class AnimationCompress
	{
	public:
		AnimationCompress(pa::Animation* pAnimation);
		~AnimationCompress() = default;

	private:
		float getError(const DirectX::XMVECTOR& origin, const DirectX::XMVECTOR& other) const;
		void fitCurve(const Animation* pAnimation, size_t boneIndex);
		
	private:
		std::vector<pa::CompressedFrame> _compressedFrames;
	};
}

