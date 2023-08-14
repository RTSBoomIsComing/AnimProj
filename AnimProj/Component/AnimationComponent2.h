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
			Layer()
			{
				lastPose.resize(50 /* hard coded, need fix */);
				currentPose.resize(50 /* hard coded, need fix */);
			}

			std::vector<std::array<CompactKey, 4>> activeKeys;
			const Animation*					   animation	  = nullptr;
			size_t								   cursor		  = 0;
			float								   playTime		  = 0.0f;
			float								   transitionTime = 0.0f;

			void transitAnimation(const Animation& animation, float transitionTime = 1.0f);
			void onUpdate(float deltaTime);

			std::vector<Transform> lastPose;
			std::vector<Transform> currentPose;
		};

	public:
		void transitAnimationUpperBody(const Animation& animation, float transitionTime = 1.0f);
		void transitAnimationLowerBody(const Animation& animation, float transitionTime = 1.0f);
		void onUpdate(World& world, Actor& owner, float deltaTime);

	private:
		Layer _upperBody{};
		Layer _lowerBody{};

		//std::vector<Transform> _resultPose;
	};
}
