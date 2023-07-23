// author: Changwan Yu
#include "pch.h"
#include "AnimationBuilder.h"

pa::AnimationBuilder::AnimationBuilder(const Skeleton& skeleton, const RawAnimation& rawAnimation)
	: _skeleton(skeleton)
	, _rawAnimation(rawAnimation)
{
	std::vector<RawAnimation::Track> compressLv1;
	for (const auto& track : rawAnimation._tracks)
	{
		compressLv1.push_back(this->removeDuplicateFrame(track));
	}

	std::vector<RawAnimation::Track> compressLv2;
	for (const auto& track : compressLv1)
	{
		compressLv2.push_back(this->fitCurveWithCatmullRom(track));
	}

}

pa::RawAnimation::Track pa::AnimationBuilder::removeDuplicateFrame(RawAnimation::Track const& track)
{
	using namespace DirectX;
	RawAnimation::Track newTrack;
	newTrack.boneID	= track.boneID;
	newTrack.type	= track.type;
	
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

pa::RawAnimation::Track pa::AnimationBuilder::fitCurveWithCatmullRom(RawAnimation::Track const& track, float threshold)
{
	using namespace DirectX;

	if (track.values.empty() || track.times.empty())
		DebugBreak();

	if (track.values.size() != track.times.size())
		DebugBreak();

	std::vector<bool>	samples(track.values.size(), false);
	samples.front()		= true;
	samples.back()		= true;


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
				float t  = track.times[between];
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

	RawAnimation::Track newTrack;
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

