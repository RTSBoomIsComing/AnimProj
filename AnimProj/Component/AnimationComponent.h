// author: Changwan Yu
#pragma once
#include "../Animation/AnimationPlayer.h"
namespace pa
{
	class AnimationComponent
	{
	public:
		AnimationComponent(ID3D11Device* device);
		~AnimationComponent();
	private:
		std::vector<AnimationPlayer>	_animationPlayers;
		std::vector<Transform>			_pose;
	};
}

