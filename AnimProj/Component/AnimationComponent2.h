// author: Changwan Yu
#pragma once
#include "../Animation/CompactKey.h"

namespace pa
{
	class Actor;
	class World;
	class Animation;
	class AnimationComponent2
	{
		struct Layer
		{
			std::vector<std::array<CompactKey, 4>> activeKeys;
			size_t								   cursor	 = 0;
			const Animation*					   animation = nullptr;
			float								   playTime	 = 0.0f;
			float								   transitionTime = 0.0f;
		};

	public:
		void onUpdate(World& world, Actor& owner, float deltaTime);
		void transitAnimationUpperBody(const Animation& animation, float transitionTime = 1.0f);
		void transitAnimationLowerBody(const Animation& animation, float transitionTime = 1.0f);

	private:
		Layer _upperBody{};
		Layer _lowerBody{};
	};
}
