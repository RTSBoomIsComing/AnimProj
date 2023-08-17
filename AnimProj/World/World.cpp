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
		std::uniform_real_distribution<float> dis(15.0f, 20.0f);

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
					{
						sceneComp->position.x = positionXZ.first;
						sceneComp->position.z = positionXZ.second;
					}

					if (CombatComponent* combatComp = actor->getComponent<CombatComponent>())
					{
						// set attack range randomly
						//combatComp->setAttackRange(dis(gen));
						const int teamID = (x < _map->getMapWidth() / 2) ? 1 : 0;
						combatComp->setTeamID(teamID);
					}
				}
			}
		}

		// shuffle order of actors
		for (size_t i = 0; i < _actors.size() / 2; i++)
		{
			if (i % 2)
				continue;

			_actors[i].swap(_actors[_actors.size() / 2 + i]);
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
		_aliveActorCount	 = 0;
		_actorCountInFrustum = 0;
		this->boneMatrixPool.clear();
		this->boneToBoneMatrixPool.clear();

		// remove actors pending kill
		for (size_t i = 0; i < _actors.size(); i++)
		{
			if (i == _actors.size() - 1)
				break;

			std::shared_ptr<Actor>& actor = _actors[i];
			if (actor->isPendingKill())
			{
				while (_actors.back()->isPendingKill())
					_actors.pop_back();

				actor.swap(_actors.back());
				_actors.pop_back();
			}
		}

		_map->clearMap();
		for (auto& actor : _actors)
		{
			if (!actor->isAlive())
				continue;

			_map->placeActor(*this, *actor);
			_aliveActorCount++;
		}

		for (auto& actor : _actors)
		{
			if (!cullActor(actor.get()))
				_actorCountInFrustum++;

			actor->onUpdate(*this, deltaTime);
		}
	}

	bool World::cullActor(Actor* actor)
	{
		constexpr float boundingShpereRadius = 5.0f;
		SceneComponent* sceneComp			 = actor->getComponent<SceneComponent>();
		assert(sceneComp);

		using namespace DirectX;
		const bool isInFrustum = _camera->checkFrustumWithSphere(XMLoadFloat3(&sceneComp->position), boundingShpereRadius);
		sceneComp->setIsCulled(!isInFrustum);
		return !isInFrustum;
	}
}