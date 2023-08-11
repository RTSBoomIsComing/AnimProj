// author: Changwan Yu
#include "pch.h"
#include "World.h"
#include "../Map/GridMap.h"
#include "../Actor/Actor.h"
#include "../Component/SceneComponent.h"
#include "../Component/BehaviorTreeComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/SkeletalMeshComponent.h"
pa::World::World()
{
	getComponentManager<SceneComponent>() = std::make_shared<ComponentManager<SceneComponent>>();
	getComponentManager<BehaviorTreeComponent>() = std::make_shared<ComponentManager<BehaviorTreeComponent>>();
	getComponentManager<MovementComponent>() = std::make_shared<ComponentManager<MovementComponent>>();
	getComponentManager<SkeletalMeshComponent>() = std::make_shared<ComponentManager<SkeletalMeshComponent>>();

	_map = std::make_shared<GridMap>(20.0f);
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

	this->updateComponents<MovementComponent>(deltaTime);

	_map->clearMap();
	for (auto& actor : _actors)
	{
		_map->placeActor(*this, actor);
	}

	this->updateComponents<BehaviorTreeComponent>(deltaTime);
}

void pa::World::initializeActorComponents(std::shared_ptr<Actor>& actor)
{
	actor->initializeComponents(*this);
}

void pa::World::setActorTransform(std::shared_ptr<Actor>& actor, DirectX::XMFLOAT3 const& position, DirectX::XMFLOAT3 const& eulerAngle)
{
	auto& sceneComp = actor->getComponent<SceneComponent>(*this);
	sceneComp.position = position;
	sceneComp.eulerAngle = eulerAngle;
}

