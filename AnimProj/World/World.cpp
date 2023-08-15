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
		_map					 = std::make_shared<GridMap>(10.0f);
		static auto behaviorTree = std::make_shared<CharacterBehaviorTree>();

		// place actors on center of sectors

		for (size_t w = 0; w < _map->getMapWidth(); w++)
		{
			for (size_t h = 0; h < _map->getMapHeight(); h++)

			{
				std::shared_ptr<MyActor> actor = std::make_shared<MyActor>();
				_actors.push_back(actor);

				if (SceneComponent* sceneComp = actor->getComponent<SceneComponent>())
				{
					std::pair<float, float> positionXZ = _map->getCellCenter(w, h);
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