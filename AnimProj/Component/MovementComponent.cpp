// author: Changwan Yu
#include "pch.h"
#include "MovementComponent.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Component/SceneComponent.h"

namespace pa
{
	void MovementComponent::onUpdate(World& world, Actor& owner, float deltaTime)
	{
		using namespace DirectX;

		SceneComponent* sceneComp = owner.getComponent<SceneComponent>();
		assert(sceneComp);

		if (0.0f == speed)
			return;

		const XMVECTOR V0 = XMLoadFloat3(&sceneComp->position);
		const XMVECTOR V1 = XMLoadFloat3(&targetPosition);

		{
			float diff = std::fabsf(targetEulerAngle.y - sceneComp->eulerAngle.y);
			float t = rotateSpeed * deltaTime / diff;
			t = std::min(1.0f, t);

			sceneComp->eulerAngle.y = sceneComp->eulerAngle.y * (1.0f - t) + targetEulerAngle.y * t;
		}

		const float distance = XMVectorGetX(XMVector3Length(V1 - V0));

		// TODO: Move this logic to SphereComponent
		if (distance < 5.0f)
			return;

		float		t		 = speed * deltaTime / distance;
		t					 = std::min(1.0f, t);

		XMVECTOR V = XMVectorLerp(V0, V1, t);
		XMStoreFloat3(&sceneComp->position, V);
	}
}
