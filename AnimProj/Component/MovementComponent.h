// author: Changwan Yu
#pragma once
namespace pa
{
	class World;
	class Actor;
	class MovementComponent
	{
	public:
		void onUpdate(World& world, std::weak_ptr<Actor> owner, float deltaTime);

	public:
		DirectX::XMFLOAT3	targetPosition		= {};
		DirectX::XMFLOAT3	targetEulerAngle	= {};
		float			 	speed				= 0.0f;
		float				angleSpeed			= 0.0f;
	};
}

