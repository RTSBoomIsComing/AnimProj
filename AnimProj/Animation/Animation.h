// author: Changwan Yu
#pragma once
#include "Keyframe.h"
namespace pa
{
	class ASF;
	class AMC;
	class Animation
	{
		friend class AnimationController;

	public:
		struct Frame
		{
			uint16_t			key		= std::numeric_limits<uint16_t>::max();

			// scale, position or rotation(quaternion)
			DirectX::XMFLOAT4	v		= {};
		};

		struct BoneAnimation
		{
			std::vector<Frame>	scale;
			std::vector<Frame>	rotation;
			std::vector<Frame>	position;
		};
	public:
		Animation() = default;
		Animation(const ASF* acclaimSkeleton, const AMC* acclaimMotion);
		~Animation() = default;

		bool				initializeAnimation(const ASF* acclaimSkeleton, const AMC* acclaimMotion);
		void				compressAnimation();
		inline size_t		getDuration()				const { return _duration; }
		inline size_t		getBoneAnimationCount()		const { return _boneAnimation.size(); }

		void				testCreateTrack();
	private:
		void				fitBoneAnimationCatmullRom(std::vector<Animation::Frame>& frames, float threshold = 0.00005f);
		void				fitBoneAnimationCatmullRomCyclic(std::vector<Animation::Frame>& frames, float threshold = 0.00005f);

	private:
		size_t							_duration			= 0;
		std::vector<BoneAnimation>		_boneAnimation;

		uint16_t						_trackCount			= 0;
		std::vector<Keyframe>			_rotationTrack;
	};
}

