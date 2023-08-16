// author: Changwan Yu
#include "pch.h"
#include "World.h"
#include "../Map/GridMap.h"
#include "../Actor/MyActor.h"
#include "../Behavior/CharacterBehaviorTree.h"
#include "../Component/SceneComponent.h"
#include "../Component/BehaviorTreeComponent.h"
#include "../Component/CombatComponent.h"
#include "../Rendering/Camera.h"

#include <random>

namespace pa
{
	World::World()
	{
		_map					 = std::make_shared<GridMap>(50.0f);
		static auto behaviorTree = std::make_shared<CharacterBehaviorTree>(*this);

		// random generator for attack range
		std::random_device					  rd;
		std::mt19937						  gen(rd());
		std::uniform_real_distribution<float> dis(19.0f, 20.0f);

		// place actors on center of sectors
		for (size_t x = 0; x < _map->getMapWidth(); x++)
		{
			for (size_t z = 0; z < _map->getMapHeight(); z++)
			{
				std::pair<float, float> positionXZ = _map->getCellCenter(x, z);

				// create MyActor
				{
					std::shared_ptr<MyActor> actor = std::make_shared<MyActor>();
					_actors.push_back(actor);
					if (BehaviorTreeComponent* behaviorTreeComp = actor->getComponent<BehaviorTreeComponent>())
						behaviorTreeComp->setBehaviorTree(behaviorTree);
					if (SceneComponent* sceneComp = actor->getComponent<SceneComponent>())
						sceneComp->position = {positionXZ.first, 0.0f, positionXZ.second};

					if (CombatComponent* combatComp = actor->getComponent<CombatComponent>())
					{
						// set attack range randomly
						combatComp->setAttackRange(dis(gen));
					}
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
		_aliveActorCount = 0;
		_map->clearMap();
		for (auto& actor : _actors)
		{
			if (!actor->isAlive())
				continue;

			_map->placeActor(*this, *actor);
			_aliveActorCount++;
		}

		this->boneMatrixPool.clear();
		this->boneToBoneMatrixPool.clear();

		for (auto& actor : _actors)
		{
			cullActor(actor.get());
			actor->onUpdate(*this, deltaTime);
		}
	}

	void World::cullActor(Actor* actor)
	{
		constexpr float boundingShpereRadius = 5.0f;
		SceneComponent* sceneComp			 = actor->getComponent<SceneComponent>();
		assert(sceneComp);

		using namespace DirectX;
		const bool isInFrustum = _camera->checkFrustumWithSphere(XMLoadFloat3(&sceneComp->position), boundingShpereRadius);
		sceneComp->setIsCulled(!isInFrustum);
	}
}