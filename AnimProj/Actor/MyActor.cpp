// author: Changwan Yu
#include "pch.h"
#include "MyActor.h"
#include "../World/World.h"
#include "../Component/SceneComponent.h"

void pa::MyActor::initializeComponents(World& world)
{
	_sceneComp = world.createComponent<SceneComponent>();
}
