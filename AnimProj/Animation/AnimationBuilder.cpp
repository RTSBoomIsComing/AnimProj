// author: Changwan Yu
#include "pch.h"
#include "AnimationBuilder.h"
#include "Animation.h"

pa::AnimationBuilder::AnimationBuilder(const Skeleton* skeleton, const Animation* animation)
	: _skeleton(skeleton)
	, _animation(animation)
{

}

pa::Animation* pa::AnimationBuilder::createCompactAnimation(Animation* animation, std::vector<bool> const& mask)
{
	return nullptr;
}
