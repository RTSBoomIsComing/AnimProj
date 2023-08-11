#include "pch.h"
#include "CharacterBehaviorTree.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Map/GridMap.h"
#include "../Component/SceneComponent.h"
#include "../Component/MovementComponent.h"

pa::CharacterBehaviorTree::CharacterBehaviorTree()
{
	std::shared_ptr<Behavior::Sequence> rootSequence = std::make_shared<Behavior::Sequence>();
	_root = rootSequence;

	rootSequence->addChild(std::make_shared<FindTarget>());
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
	std::shared_ptr<GridMap> map = world.getDefaultMap();

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
			XMStoreFloat3(&ownerLocked->getComponent<MovementComponent>(world).targetPosition, VotherPosition);

			return true;
		}
	}
	return false;
}

bool pa::CharacterBehaviorTree::MoveToTarget::onUpdate(World& world, std::weak_ptr<Actor> owner)
{
	return false;
}
