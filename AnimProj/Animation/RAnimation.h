// author: Changwan Yu
#pragma once

namespace pa
{
	class ASF;
	class AMC;
	class RAnimation
	{
		friend class MyApplication;
	public:
		struct Frame
		{
			uint32_t			key		= std::numeric_limits<uint32_t>::max();

			// scale, position or rotation(quaternion)
			DirectX::XMFLOAT4	v		= {};
		};

		struct BoneAnimation
		{
			std::vector<RAnimation::Frame>	scale;
			std::vector<RAnimation::Frame>	rotation;
			std::vector<RAnimation::Frame>	position;
		};
	public:
		RAnimation() = default;
		RAnimation(const ASF* acclaimSkeleton, const AMC* acclaimMotion);
		~RAnimation() = default;

		bool initializeAnimation(const ASF* acclaimSkeleton, const AMC* acclaimMotion);

	private:
		size_t							_duration			= 0;
		std::vector<BoneAnimation>		_boneAnimation;
	};
}

