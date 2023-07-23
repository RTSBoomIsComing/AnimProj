// author: Changwan Yu
#include "pch.h"
#include "AnimationBuilder.h"
#include "Animation.h"

pa::AnimationBuilder::AnimationBuilder(const Skeleton& skeleton, const RawAnimation& rawAnimation)
	: _skeleton(skeleton)
	, _rawAnimation(rawAnimation)
{

}

pa::Animation* pa::AnimationBuilder::createCompactAnimation(Animation* animation, std::vector<bool> const& mask)
{
	return nullptr;
}
