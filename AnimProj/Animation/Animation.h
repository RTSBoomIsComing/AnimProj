// author: Changwan Yu
#pragma once

namespace pa
{
	class ASF;
	class AMC;
	class Skeleton;
	class Animation
	{
	public:
		struct Pose
		{
			std::vector<DirectX::XMFLOAT4>	scale;
			std::vector<DirectX::XMFLOAT4>	rotation;
			std::vector<DirectX::XMFLOAT4>	position;
		};

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
		inline size_t		getDuration() const { return _duration; }
		DirectX::XMVECTOR 	getBoneRotation(size_t boneIndex, uint32_t key, uint32_t offset = 0);
		DirectX::XMVECTOR	playBoneAnimationCatmullRom(std::vector<Animation::Frame> const& frames, uint32_t key, uint32_t offset = 0);
	private:
		void				fitBoneAnimationCatmullRom(std::vector<Animation::Frame>& frames, float threshold = 0.00005f);

	private:
		size_t							_duration			= 0;
		std::vector<BoneAnimation>		_boneAnimation;
	};
}

