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
		inline bool isMovable() const { return _isMovable; }
		inline void setMovable(bool isMovable) { _isMovable = isMovable; }

	public:
		DirectX::XMFLOAT3 targetPosition = {};

	private:
		float speed		  = 5.0f;
		float rotateSpeed = DirectX::XM_PIDIV2;
		bool  _isMovable  = false;
		bool  _isMoving	  = false;
		bool  _isRotating = false;
	};
}