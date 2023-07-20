// author: Changwan Yu
#pragma once
#include "Keyframe.h"
#include "CompactKeyframe.h"
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
			// scale, position or rotation(quaternion)
			uint16_t			keytime		= std::numeric_limits<uint16_t>::max();
			uint16_t			id			= std::numeric_limits<uint16_t>::max();
			DirectX::XMFLOAT4	v			= {};
		};

		struct TrackDescriptor
		{
			uint16_t	id		: 14;

			// scale : 0, rotation: 1, tanslation: 2
			uint16_t	channel	: 2;	
		};

		using Track			= std::vector<Frame>;

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

		void				createDetailedTrack();
	private:
		void				fitBoneAnimationCatmullRom(std::vector<Animation::Frame>& frames, float threshold = 0.00005f);
		void				fitBoneAnimationCatmullRomCyclic(std::vector<Animation::Frame>& frames, float threshold = 0.00005f);
		bool				validateDetailedTrack();
	private:
		size_t							_duration			= 0;
		std::vector<BoneAnimation>		_boneAnimation;

		// raw animation tracks
		std::vector<Track>				_scaleTracks;
		std::vector<Track>				_rotationTracks;
		std::vector<Track>				_translationTracks;

		std::vector<TrackDescriptor>	_trackDescriptors;
		std::vector<Keyframe>			_detailedTrack;
		std::vector<CompactKeyframe>	_compactTrack;
	};
}

