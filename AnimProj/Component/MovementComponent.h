// author: Changwan Yu
#pragma once
namespace pa
{
	class World;
	class Actor;
	class MovementComponent
	{
	public:
		void		onUpdate(World& world, Actor& owner, float deltaTime);
		inline bool isMoving() const { return _isMoving; }
		inline bool isRotating() const { return _isRotating; }

	public:
		DirectX::XMFLOAT3 targetPosition = {};
		float			  speed			 = 1.0f;
		float			  rotateSpeed	 = DirectX::XM_PIDIV2;

	private:
		bool _isMoving = false;
		bool _isRotating = false;
	};
}