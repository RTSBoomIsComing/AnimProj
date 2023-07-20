// author: Changwan Yu
#pragma once
//#include "Keyframe.h"
#include "CompactKeyframe.h"
namespace pa
{
	class ASF;
	class AMC;
	class Animation
	{
		friend class AnimationController;

	public:
		struct Keyframe
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

		using Track			= std::vector<Keyframe>;

	public:
		Animation() = default;
		Animation(const ASF* acclaimSkeleton, const AMC* acclaimMotion);
		~Animation() = default;

		bool				initializeAnimation(const ASF* acclaimSkeleton, const AMC* acclaimMotion);
		void				compressAnimation();
		inline uint16_t		getDuration()				const { return _duration; }
		inline uint16_t		getBoneCount()				const { return _boneCount; }

		void				createDetailedStream();
		void				createCompactStream();
	private:
		void				fitBoneAnimationCatmullRom(std::vector<Animation::Keyframe>& frames, float threshold = 0.00005f);
		void				fitBoneAnimationCatmullRomCyclic(std::vector<Animation::Keyframe>& frames, float threshold = 0.00005f);
		bool				validateDetailedStream();
	private:
		uint16_t							_duration			= 0;
		uint16_t						_boneCount			= 0;

		// raw animation tracks
		std::vector<Track>				_scaleTracks;
		std::vector<Track>				_rotationTracks;
		std::vector<Track>				_translationTracks;

		std::vector<TrackDescriptor>	_scaleTrackDescriptors;
		std::vector<TrackDescriptor>	_rotationTrackDescriptors;
		std::vector<TrackDescriptor>	_translationTrackDescriptors;




		std::vector<Keyframe>			_detailedStream;
		std::vector<CompactKeyframe>	_compactStream;
	};
}

