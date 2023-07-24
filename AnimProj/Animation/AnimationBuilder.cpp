// author: Changwan Yu
#include "pch.h"
#include "AnimationBuilder.h"
#include "CompactAnimation.h"
#include "RawAnimation.h"
#include "Skeleton.h"

pa::AnimationBuilder::AnimationBuilder(const Skeleton& skeleton, const RawAnimation& rawAnimation)
	: _skeleton(skeleton)
	, _rawAnimation(rawAnimation)
{
	//for (const auto& track : rawAnimation._tracks)
	//{
	//	_tracks.push_back(this->removeDuplicateFrame(track));
	//}
	_tracks = rawAnimation._tracks;

	std::vector<AnimationTrack> compressed;
	for (const auto& track : _tracks)
	{
		compressed.push_back(this->fitCurveWithCatmullRom(track));
	}

	_tracks = std::move(compressed);
}

pa::AnimationTrack pa::AnimationBuilder::removeDuplicateFrame(AnimationTrack const& track)
{
	using namespace DirectX;
	AnimationTrack newTrack;
	newTrack.boneID = track.boneID;
	newTrack.type = track.type;

	newTrack.values.push_back(track.values.front());
	newTrack.times.push_back(track.times.front());

	for (size_t i = 1; i < track.values.size() - 1; i++)
	{
		XMVECTOR V0 = XMLoadFloat4(&track.values[i - 1]);
		XMVECTOR V1 = XMLoadFloat4(&track.values[i]);

		if (!XMVector4Equal(V0, V1))
		{
			newTrack.values.push_back(track.values[i]);
			newTrack.times.push_back(track.times[i]);
			continue;
		}

		XMVECTOR V2 = XMLoadFloat4(&track.values[i + 1]);
		if (!XMVector4Equal(V1, V2))
		{
			newTrack.values.push_back(track.values[i]);
			newTrack.times.push_back(track.times[i]);
		}
	}

	newTrack.values.push_back(track.values.back());
	newTrack.times.push_back(track.times.back());

	if (newTrack.values.size() != newTrack.times.size())
		DebugBreak();

	return newTrack;
}

pa::AnimationTrack pa::AnimationBuilder::fitCurveWithCatmullRom(AnimationTrack const& track, float threshold)
{
	using namespace DirectX;

	if (track.values.empty() || track.times.empty())
		DebugBreak();

	if (track.values.size() != track.times.size())
		DebugBreak();

	std::vector<bool>	samples(track.values.size(), false);
	samples.front() = true;
	samples.back() = true;


	while (true)
	{
		std::vector<size_t>	midPoints;
		std::vector<float>	errors;

		size_t P0 = 0;

		auto iterator = std::find(samples.begin(), samples.end(), true);
		size_t P1 = std::distance(samples.begin(), iterator);

		iterator = std::find(++iterator, samples.end(), true);
		size_t P2 = std::distance(samples.begin(), iterator);

		iterator = std::find(++iterator, samples.end(), true);
		size_t P3 = (iterator != samples.end()) ? std::distance(samples.begin(), iterator) : P2;

		while (P1 < P2)
		{
			midPoints.push_back((P1 + P2) / 2);
			errors.push_back(0.0f);

			for (size_t between = P1 + 1; between < P2; between++)
			{
				float t = track.times[between];
				float t1 = track.times[P1];
				float t2 = track.times[P2];

				const float weight = (t - t1) / (t2 - t1);
				XMVECTOR catmullRom = XMVectorCatmullRom(
					XMLoadFloat4(&track.values[P0]),
					XMLoadFloat4(&track.values[P1]),
					XMLoadFloat4(&track.values[P2]),
					XMLoadFloat4(&track.values[P3]), weight);

				const XMVECTOR	difference = XMLoadFloat4(&track.values[between]) - catmullRom;
				const float error = XMVectorGetX(XMVector4LengthSq(difference));
				errors.back() += error;

				//if (errors.back() < error)
				//{
				//	errors.back() = error;
				//	midPoints.back() = between;
				//}
			}

			P0 = P1;
			P1 = P2;
			P2 = P3;

			if (iterator != samples.end())
				iterator = std::find(++iterator, samples.end(), true);

			if (iterator != samples.end())
				P3 = std::distance(samples.begin(), iterator);
		}

		if (errors.empty())
			break;

		auto findMaxError = std::max_element(errors.begin(), errors.end());
		if (*findMaxError < threshold)
			break;

		samples[midPoints[std::distance(errors.begin(), findMaxError)]] = true;
	}

	AnimationTrack newTrack;
	newTrack.boneID = track.boneID;
	newTrack.type = track.type;

	for (size_t i = 0; i < samples.size(); i++)
	{
		if (samples[i])
		{
			newTrack.values.push_back(track.values[i]);
			newTrack.times.push_back(track.times[i]);
		}
	}

	if (track.values.empty() || track.times.empty())
		DebugBreak();

	if (track.values.size() != track.times.size())
		DebugBreak();

	return newTrack;
}

pa::CompactAnimation pa::AnimationBuilder::buildCompactAnimation(std::vector<AnimationTrack> const& tracks)
{
	using namespace DirectX;

	std::vector<AnimationTrackHeader> trackHeaders;
	for (const auto& track : tracks)
	{
		AnimationTrackHeader trackHeader;
		trackHeader.boneID = track.boneID;

		const uint16_t trackType = static_cast<uint16_t>(track.type);
		trackHeader.type = static_cast<AnimationType>(trackType);

		trackHeaders.push_back(trackHeader);
	}

	std::vector<ExtendedKey> keyframeBuilders = mergeTracks2(tracks);

	std::vector<CompactKeyframe> keyframes;
	keyframes.reserve(keyframeBuilders.size());

	std::vector<uint16_t> trackIndices;
	trackIndices.reserve(keyframeBuilders.size());

	for (const ExtendedKey& keyframeBuilder : keyframeBuilders)
	{
		if (keyframeBuilder.isQuaternion)
		{
			XMVECTOR Q = XMLoadFloat4(&keyframeBuilder.value);
			CompactKeyframe keyframe = CompactKeyframe::createFromQuaternion(Q);
			keyframe.keytime = keyframeBuilder.keyTime;
		
			keyframes.push_back(keyframe);
		}
		else
		{
			XMVECTOR V = XMLoadFloat4(&keyframeBuilder.value);
			CompactKeyframe keyframe = CompactKeyframe::createFromVector(V);
			keyframe.keytime = keyframeBuilder.keyTime;
		
			keyframes.push_back(keyframe);
		}
		
		trackIndices.push_back(keyframeBuilder.trackID);
	}

	CompactAnimation compactAnimation;
	compactAnimation._keyframes = std::move(keyframes);
	compactAnimation._trackIndices = std::move(trackIndices);
	compactAnimation._trackHeaders = std::move(trackHeaders);

	return compactAnimation;
}

std::vector<pa::AnimationBuilder::ExtendedKey> pa::AnimationBuilder::mergeTracks(std::vector<AnimationTrack> const& tracks)
{
	size_t keyframeCount = 0;
	for (const auto& track : tracks)
	{
		keyframeCount += track.values.size();
	}

	std::vector<ExtendedKey> keyframeBuilders;
	keyframeBuilders.reserve(keyframeCount);

	for (size_t trackID = 0; trackID < tracks.size(); trackID++)
	{
		const auto& track = tracks[trackID];

		// first key
		ExtendedKey keyframeBuilder = {};
		keyframeBuilder.prevKeyTime = -2;
		keyframeBuilder.keyTime = track.times.front();
		keyframeBuilder.trackID = trackID;
		keyframeBuilder.value = track.values.front();
		keyframeBuilder.isQuaternion = (AnimationTrack::Type::Rotation == track.type);

		keyframeBuilders.push_back(keyframeBuilder);
		keyframeBuilders.push_back(keyframeBuilder);

		// second key
		keyframeBuilder = {};
		keyframeBuilder.prevKeyTime = -1;
		keyframeBuilder.keyTime = track.times[1];
		keyframeBuilder.trackID = trackID;
		keyframeBuilder.value = track.values[1];
		keyframeBuilder.isQuaternion = (AnimationTrack::Type::Rotation == track.type);
		keyframeBuilders.push_back(keyframeBuilder);

		if (track.values.size() == 2)
			keyframeBuilders.push_back(keyframeBuilder);

		for (size_t i = 2; i < track.values.size(); i++)
		{
			ExtendedKey keyframeBuilder = {};
			keyframeBuilder.prevKeyTime = track.times[i - 2];
			keyframeBuilder.keyTime = track.times[i];
			keyframeBuilder.trackID = trackID;
			keyframeBuilder.value = track.values[i];
			keyframeBuilder.isQuaternion = (AnimationTrack::Type::Rotation == track.type);

			keyframeBuilders.push_back(keyframeBuilder);

			if (i == track.values.size() - 1)
			{
				keyframeBuilders.push_back(keyframeBuilder);
			}
		}
	}

	std::sort(keyframeBuilders.begin(), keyframeBuilders.end(), SortingKeyframeBuilderLess);

	return keyframeBuilders;
}

std::vector<pa::AnimationBuilder::ExtendedKey> pa::AnimationBuilder::mergeTracks2(std::vector<AnimationTrack> const& tracks)
{
	size_t keyframeCount = 0;
	for (const auto& track : tracks)
	{
		keyframeCount += track.values.size();
	}

	std::vector<ExtendedKey> keyframeBuilders;
	keyframeBuilders.reserve(keyframeCount);

	std::vector<uint16_t> cursors(tracks.size(), 0ui16);
	for (uint16_t trackID = 0; trackID < tracks.size(); trackID++)
	{
		const auto& track = tracks[trackID];

		ExtendedKey keyframeBuilder = {};
		keyframeBuilder.prevKeyTime = -2;
		keyframeBuilder.trackID = trackID;
		keyframeBuilder.keyTime = track.times.front();
		keyframeBuilder.value = track.values.front();
		keyframeBuilder.isQuaternion = (AnimationTrack::Type::Rotation == track.type);


		keyframeBuilders.push_back(keyframeBuilder);
		keyframeBuilders.push_back(keyframeBuilder);

		keyframeBuilder.keyTime = track.times[1];
		keyframeBuilder.value   = track.values[1];
		keyframeBuilders.push_back(keyframeBuilder);

		if (2 < track.times.size())
		{
			keyframeBuilder.keyTime = track.times[2];
			keyframeBuilder.value	= track.values[2];
		}

		keyframeBuilders.push_back(keyframeBuilder);
		cursors[trackID] = 2;

	}

	uint16_t _duration = tracks.front().times.back();
	for (uint16_t currentKeytime = 0; currentKeytime < _duration; currentKeytime++)
	{
		for (uint16_t trackID = 0; trackID < tracks.size(); trackID++)
		{
			const auto& track = tracks[trackID];
			uint16_t& cursor = cursors[trackID];
			if (track.times.size() == cursor)
				continue;

			if (track.times[cursor - 1] == currentKeytime)
			{
				cursor += 1;
				//assert(cursor < currentTrack.size());
				if (cursor == track.times.size())
				{
					ExtendedKey keyframeBuilder = {};
					keyframeBuilder.prevKeyTime = 0;
					keyframeBuilder.trackID = trackID;
					keyframeBuilder.keyTime = track.times.back();
					keyframeBuilder.value = track.values.back();
					keyframeBuilder.isQuaternion = (AnimationTrack::Type::Rotation == track.type);

					keyframeBuilders.push_back(keyframeBuilder);
				}
				else
				{
					ExtendedKey keyframeBuilder = {};
					keyframeBuilder.prevKeyTime = 0;
					keyframeBuilder.trackID = trackID;
					keyframeBuilder.keyTime = track.times[cursor];
					keyframeBuilder.value = track.values[cursor];
					keyframeBuilder.isQuaternion = (AnimationTrack::Type::Rotation == track.type);

					keyframeBuilders.push_back(keyframeBuilder);
				}
			}
		}
	}

	return keyframeBuilders;
}

bool pa::AnimationBuilder::SortingKeyframeBuilderLess(const ExtendedKey& a, const ExtendedKey& b)
{
	return a.prevKeyTime < b.prevKeyTime
		|| (a.prevKeyTime == b.prevKeyTime
			&& a.trackID < b.trackID);
}

pa::CompactAnimation pa::AnimationBuilder::createFullBodyAnimation() const
{
	return buildCompactAnimation(_tracks);
}

pa::CompactAnimation pa::AnimationBuilder::createUpperBodyAnimation() const
{
	std::vector<AnimationTrack> upperBodyTracks;
	for (const auto& track : _tracks)
	{
		uint16_t boneID = track.boneID;
		if (_skeleton.getUpperBodyMask()[boneID])
			upperBodyTracks.push_back(track);
	}

	return buildCompactAnimation(upperBodyTracks);
}

pa::CompactAnimation pa::AnimationBuilder::createLowerBodyAnimation() const
{
	std::vector<AnimationTrack> lowerBodyTracks;
	for (const auto& track : _tracks)
	{
		uint16_t boneID = track.boneID;
		if (_skeleton.getLowerBodyMask()[boneID])
			lowerBodyTracks.push_back(track);
	}

	return buildCompactAnimation(lowerBodyTracks);
}