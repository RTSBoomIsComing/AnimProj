// author: Changwan Yu
#pragma once
class PrimitiveAnimation
{
	friend class Acclaim;
public:
	struct Keyframe
	{
		// scale, position or rotation(quaternion)
		DirectX::XMFLOAT4	v = {};

		uint16_t			keytime;
		uint16_t			id		: 14;

		// scale: 0, rotation: 1, translation: 2
		uint16_t			channel	: 2;
	};

	using AnimationTrack = std::vector<Keyframe>;
public:
	PrimitiveAnimation() = default;
	~PrimitiveAnimation() = default;

private:
	std::vector<AnimationTrack> _tracks;
};

