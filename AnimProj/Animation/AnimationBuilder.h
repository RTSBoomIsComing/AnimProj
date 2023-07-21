#pragma once
// author: Changwan Yu
// Construct with Raw Animation
// Then build compact animation stream
// Manage Skeleton layer
// 

namespace pa
{
	class Skeleton;
	class Animation;
	class AnimationBuilder
	{
	public:
		AnimationBuilder(const Skeleton* skeleton, const Animation* animation);
		~AnimationBuilder() = default;

		static Animation* createDetailedAnimation(Animation* animation, std::vector<bool> const& mask);
		static Animation* createCompactAnimation(Animation* animation, std::vector<bool> const& mask);

	private:
		const Skeleton* const	_skeleton;
		const Animation* const	_animation;
	};
}

