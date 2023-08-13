// author: Changwan Yu
#include "pch.h"
#include "SphereComponent.h"
#include "../World/World.h"
#include "../Map/GridMap.h"
#include "../Actor/Actor.h"

#include "../Component/SceneComponent.h"

void pa::SphereComponent::onUpdate(World& world, Actor& owner, float deltaTime)
{
	using namespace DirectX;

	SceneComponent* sceneComponent = owner.getComponent<SceneComponent>();
	auto map = world.getDefaultMap();

	auto cellCoordinate = map->getCellCoordinate(world, owner);
	auto& actors = map->getCell(cellCoordinate.first, cellCoordinate.second);

	XMVECTOR Vposition = XMLoadFloat3(&sceneComponent->position);
	for (Actor* other : actors)
	{
		const SceneComponent* otherSceneComp = other->getComponent<SceneComponent>();
		XMVECTOR VotherPosition = XMLoadFloat3(&otherSceneComp->position);

		const float distance = XMVectorGetX(XMVector3Length(Vposition - VotherPosition));
		if (distance < _radius)
		{
			// collision
			//std::cout << "collision" << std::endl;
			//std::cout << "distance: " << distance << std::endl;
			//std::cout << "radius: " << _radius << std::endl;
			//std::cout << "position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
			//std::cout << "otherPosition: " << otherPosition.x << ", " << otherPosition.y << ", " << otherPosition.z << std::endl;
			//std::cout << std::endl;
		}
	}
}
