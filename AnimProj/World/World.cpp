// author: Changwan Yu
#include "pch.h"
#include "World.h"
#include "../Map/GridMap.h"
#include "../Actor/Actor.h"
#include "../Component/SceneComponent.h"
#include "../Component/BehaviorTreeComponent.h"
pa::World::World()
{
	getComponentManager<SceneComponent>() = std::make_shared<ComponentManager<SceneComponent>>();
	getComponentManager<BehaviorTreeComponent>() = std::make_shared<ComponentManager<BehaviorTreeComponent>>();

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

	std::vector<BehaviorTreeComponent>&	behaviorTreeComponents = this->getComponents<BehaviorTreeComponent>();
	std::vector<std::weak_ptr<Actor>>&	owners = this->getOwners<BehaviorTreeComponent>();
	for (size_t i = 0; i < behaviorTreeComponents.size(); i++)
	{
		auto& behaviorTreeComp = behaviorTreeComponents[i];
		auto& owner = owners[i];
		behaviorTreeComp.onUpdate(*this, owner, deltaTime);
	}

}
