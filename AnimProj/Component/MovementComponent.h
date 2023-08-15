// author: Changwan Yu
#pragma once
namespace pa
{
	class World;
	class Actor;
	class MovementComponent
	{
	public:
		void onUpdate(World& world, Actor& owner, float deltaTime);

	public:
		DirectX::XMFLOAT3	targetPosition		= {};
		float			 	speed				= 0.0f;
		float				rotateSpeed			= DirectX::XM_PIDIV4;
	};
}

