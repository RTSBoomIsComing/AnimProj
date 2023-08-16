// author: Changwan Yu
#include "pch.h"
#include "World.h"
#include "../Map/GridMap.h"
#include "../Actor/MyActor.h"
#include "../Behavior/CharacterBehaviorTree.h"
#include "../Component/SceneComponent.h"
#include "../Component/BehaviorTreeComponent.h"

namespace pa
{
	World::World()
	{
		_map					 = std::make_shared<GridMap>(40.0f);
		static auto behaviorTree = std::make_shared<CharacterBehaviorTree>(*this);

		// place two actors on sector
		//std::shared_ptr<MyActor> actor = std::make_shared<MyActor>();
		//_actors.push_back(actor);
		//if (SceneComponent* sceneComp = actor->getComponent<SceneComponent>())
		//{
		//	std::pair<float, float> positionXZ = _map->getCellCenter(0, 0);
		//	sceneComp->position				   = {positionXZ.first, 0.0f, positionXZ.second};
		//}
		//if (BehaviorTreeComponent* behaviorTreeComp = actor->getComponent<BehaviorTreeComponent>())
		//{
		//	behaviorTreeComp->setBehaviorTree(behaviorTree);
		//}
		//actor = std::make_shared<MyActor>();
		//_actors.push_back(actor);
		//if (SceneComponent* sceneComp = actor->getComponent<SceneComponent>())
		//{
		//	std::pair<float, float> positionXZ = _map->getCellCenter(4, 4);
		//	sceneComp->position				   = {positionXZ.first, 0.0f, positionXZ.second};
		//}
		//if (BehaviorTreeComponent* behaviorTreeComp = actor->getComponent<BehaviorTreeComponent>())
		//{
		//	behaviorTreeComp->setBehaviorTree(behaviorTree);
		//}

		// place actors on center of sectors
		for (size_t x = 0; x < _map->getMapWidth(); x++)
		{
			for (size_t z = 0; z < _map->getMapHeight(); z++)
			{
				std::shared_ptr<MyActor> actor = std::make_shared<MyActor>();
				_actors.push_back(actor);

				if (SceneComponent* sceneComp = actor->getComponent<SceneComponent>())
				{
					std::pair<float, float> positionXZ = _map->getCellCenter(x, z);
					sceneComp->position				   = {positionXZ.first, 0.0f, positionXZ.second};
				}

				if (BehaviorTreeComponent* behaviorTreeComp = actor->getComponent<BehaviorTreeComponent>())
				{
					behaviorTreeComp->setBehaviorTree(behaviorTree);
				}
			}
		}
	}

	World::~World()
	{
	}

	void World::startGame()
	{
		for (auto& actor : _actors)
		{
			actor->onStartGame(*this);
		}
	}

	void World::update(float deltaTime)
	{
		_map->clearMap();
		for (auto& actor : _actors)
		{
			if (!actor->isAlive())
				continue;

			_map->placeActor(*this, *actor);
		}

		this->boneMatrixPool.clear();
		this->boneToBoneMatrixPool.clear();

		for (auto& actor : _actors)
		{
			actor->onUpdate(*this, deltaTime);
		}
	}
}