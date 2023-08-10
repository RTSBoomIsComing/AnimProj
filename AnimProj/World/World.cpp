// author: Changwan Yu
#include "pch.h"
#include "World.h"
#include "../Map/GridMap.h"
#include "../Actor/Actor.h"
#include "../Component/SceneComponent.h"
pa::World::World()
{
	getComponentManager<SceneComponent>() = std::make_shared<ComponentManager<SceneComponent>>();

	_map = std::make_shared<GridMap>(5.0f);
}

pa::World::~World()
{
}

void pa::World::startGame()
{
	for (auto& actor : _actors)
	{
		actor->onStartGame(*this);
	}
}

void pa::World::update(float deltaTime)
{
	for (auto& actor : _actors)
	{
		actor->onUpdate(*this, deltaTime);
	}

	_map->clearMap();
	for (auto& actor : _actors)
	{
		_map->placeActor(*this, actor);
	}
}
