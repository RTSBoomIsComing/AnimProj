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

		const XMVECTOR V0	= XMLoadFloat3(&sceneComp->position);
		const XMVECTOR V1	= XMLoadFloat3(&targetPosition);
		const XMVECTOR Vdir = XMVector3Normalize(V1 - V0);


		XMVECTOR Forward = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		Forward			 = XMVector3Transform(Forward, XMMatrixRotationY(sceneComp->eulerAngle.y));

		const XMVECTOR Axis = XMVector3Cross(Forward, Vdir);
		float		   dot	= XMVectorGetX(XMVector3Dot(Forward, Vdir));
		dot					= std::min(dot, 1.0f);
		dot					= std::max(dot, -1.0f);
		_isRotating			= (dot != 1.0f);

		float deltaAngle = std::acosf(dot);

		deltaAngle = std::min(deltaAngle, rotateSpeed * deltaTime);
		if (XMVectorGetY(Axis) < 0.0f)
			deltaAngle = -1.0f * deltaAngle;

		sceneComp->eulerAngle.y = sceneComp->eulerAngle.y + deltaAngle;


		float deltaMove = XMVectorGetX(XMVector3Length(V1 - V0));

		// TODO: Move this logic to SphereComponent or Attack Behavior
		_isMoving = deltaMove >= 5.0f;

		if (!_isMoving)
			return;

		deltaMove = std::min(deltaMove, speed * deltaTime);

		XMVECTOR V = V0 + Vdir * deltaMove;
		XMStoreFloat3(&sceneComp->position, V);
	}
}
