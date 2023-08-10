// author: Changwan Yu
#include "pch.h"
#include "SphereComponent.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Map/GridMap.h"

#include "../Component/SceneComponent.h"

void pa::SphereComponent::onUpdate(World& world, std::weak_ptr<Actor> owner, float deltaTime)
{
	using namespace DirectX;

	if (owner.expired())
		DebugBreak();

	auto ownerLocked = owner.lock();
	if (!ownerLocked)
		DebugBreak();

	auto& sceneComponent = ownerLocked->getComponent<SceneComponent>(world);
	auto map = world.getDefaultMap();

	auto cellCoordinate = map->getCellCoordinate(world, ownerLocked);
	auto& actors = map->getCell(cellCoordinate.first, cellCoordinate.second);

	XMVECTOR Vposition = XMLoadFloat3(&sceneComponent.position);
	for (auto& actor : actors)
	{
		if (actor.expired())
			DebugBreak();

		auto actorLocked = actor.lock();
		if (!actorLocked)
			DebugBreak();

		if (actorLocked == ownerLocked)
			continue;

		const SceneComponent& otherSceneComp = actorLocked->getComponent<SceneComponent>(world);
		XMVECTOR VotherPosition = XMLoadFloat3(&otherSceneComp.position);

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
