#include "pch.h"
#include "CharacterBehaviorTree.h"
#include "../Rendering/Character.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Component/SceneComponent.h"
#include "../Map/GridMap.h"

pa::CharacterBehaviorTree::CharacterBehaviorTree()
{
	std::shared_ptr<Behavior::Sequence> rootSequence = std::make_shared<Behavior::Sequence>();
	_root = rootSequence;

	//rootSequence->addChild(std::make_shared<PrintID>(actor));
	//rootSequence->addChild(std::make_shared<PrintID>(actor));
	//rootSequence->addChild(std::make_shared<PrintID>(actor));
}

bool pa::CharacterBehaviorTree::FindTarget::onUpdate(World& world, std::weak_ptr<Actor> owner)
{
	using namespace DirectX;

	constexpr float _radius = 20.0f;

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
			return true;
			// Test
			std::cout << "collision" << std::endl;
			//std::cout << "distance: " << distance << std::endl;
			//std::cout << "radius: " << _radius << std::endl;
			//std::cout << "position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
			//std::cout << "otherPosition: " << otherPosition.x << ", " << otherPosition.y << ", " << otherPosition.z << std::endl;
			//std::cout << std::endl;
		}
	}
	return false;
}

bool pa::CharacterBehaviorTree::MoveToTarget::onUpdate(World& world, std::weak_ptr<Actor> owner)
{
	return false;
}
