// author: Changwan Yu
#include "pch.h"
#include "MovementComponent.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Component/SceneComponent.h"

namespace pa
{
	void MovementComponent::onUpdate(World& world, std::weak_ptr<Actor> owner, float deltaTime)
	{
		using namespace DirectX;
		if (0.0f == speed)
			return;

		if (owner.expired())
			DebugBreak();

		std::shared_ptr<Actor> ownerLocked = owner.lock();
		if (!ownerLocked)
			DebugBreak();

		auto& sceneComp = ownerLocked->getComponent<SceneComponent>(world);
		XMVECTOR V0 = XMLoadFloat3(&sceneComp.position);
		XMVECTOR V1 = XMLoadFloat3(&targetPosition);

		float t = speed * deltaTime / XMVectorGetX(XMVector3Length(V1 - V0));
		t = std::min(1.0f, t);

		XMVECTOR V = XMVectorLerp(V0, V1, t);
		XMStoreFloat3(&sceneComp.position, V);
	}
}

