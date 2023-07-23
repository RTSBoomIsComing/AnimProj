#pragma once
// author: Changwan Yu
// Construct with Raw Animation
// Then build compact animation stream
// Manage Skeleton layer
// 

namespace pa
{
	class Skeleton;
	class RawAnimation;
	class AnimationBuilder
	{
	public:
		AnimationBuilder(const Skeleton& skeleton, const RawAnimation& rawAnimation);
		~AnimationBuilder() = default;

		static Animation* createDetailedAnimation(Animation* animation, std::vector<bool> const& mask);
		static Animation* createCompactAnimation(Animation* animation, std::vector<bool> const& mask);

	private:
		const Skeleton&		_skeleton;
		const RawAnimation& _rawAnimation;
	};
}

