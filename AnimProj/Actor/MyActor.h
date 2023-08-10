// author: Changwan Yu
#pragma once
#include "Actor.h"
#include "../Component/Component.h"
namespace pa
{
	class SceneComponent;
	class MyActor : public Actor
	{
	public:
		virtual void initializeComponents(World& world)			override;
		virtual void onStartGame(World& world)					override;
		virtual void onUpdate(World& world, float deltaTime)	override;
	private:
	};
}

