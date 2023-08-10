// author: Changwan Yu
#include "pch.h"
#include "MyActor.h"
#include "../World/World.h"
#include "../Component/SceneComponent.h"

void pa::MyActor::initializeComponents(World& world)
{
	setComponentHandle<SceneComponent>(world.createComponent<SceneComponent>(weak_from_this()));
}

void pa::MyActor::onStartGame(World& world)
{
}

void pa::MyActor::onUpdate(World& world, float deltaTime)
{
}
