// author: Changwan Yu
#include "pch.h"
#include "World.h"
#include "../Map/GridMap.h"
#include "../Actor/MyActor.h"

pa::World::World()
{
	_map = std::make_shared<GridMap>(20.0f);
	
	for (int i = 0; i < 300; i++)
	{
		std::shared_ptr<MyActor> actor = std::make_shared<MyActor>();
		_actors.push_back(actor);

		if (SceneComponent* sceneComp = actor->getComponent<SceneComponent>())
		{
			sceneComp->position = {5.0f * (i % 20 - 10), 0.0f, 5.0f * (i / 20)};
		}
	}
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
	_map->clearMap();
	for (auto& actor : _actors)
	{
		_map->placeActor(*this, *actor);
	}

	this->boneMatrixPool.clear();
	this->boneToBoneMatrixPool.clear();

	for (auto& actor : _actors)
	{
		actor->onUpdate(*this, deltaTime);
	}
}
