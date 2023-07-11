// author: Changwan Yu
// Character has a skeleton and can hold several animations that are compatible with the skeleton

#pragma once
#include "Skeleton.h"
#include "../Animation/Animation.h"
namespace pa
{
	class Character
	{
	public:
		Character() = default;
		~Character() = default;

	public:
		void update(float deltaTime);
		void render(ID3D11DeviceContext* pDeviceContext);

		void transitState();

	private:
		Skeleton*				_skeleton;
		std::vector<Animation*>	_animations;
	};
}


