// author: Changwan Yu
#include "pch.h"
#include "MyActor.h"
#include "../World/World.h"
#include "../Component/SceneComponent.h"

void pa::MyActor::initializeComponents(World& world)
{
	initializeComponent<SceneComponent>(world);
}

void pa::MyActor::onStartGame(World& world)
{
	getComponent<SceneComponent>(world).position = { 1.0f, 1.0f, 1.0f };
}

void pa::MyActor::onUpdate(World& world, float deltaTime)
{
}
