// author: Changwan Yu
#pragma once

namespace pa
{
	class ASF;
	class AMC;
	class Animation
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
			std::vector<Animation::Frame>	scale;
			std::vector<Animation::Frame>	rotation;
			std::vector<Animation::Frame>	position;
		};
	public:
		Animation() = default;
		Animation(const ASF* acclaimSkeleton, const AMC* acclaimMotion);
		~Animation() = default;

		bool initializeAnimation(const ASF* acclaimSkeleton, const AMC* acclaimMotion);
		void compressAnimation();
	private:
		void fitBoneAnimationRotation(std::vector<Animation::Frame>& rotations, float threshold = 0.01f);
		float getError(const DirectX::XMVECTOR& origin, const DirectX::XMVECTOR& other) const;

	private:
		size_t							_duration			= 0;
		std::vector<BoneAnimation>		_boneAnimation;
	};
}

