// author: Changwan Yu
// Character has a skeleton and can hold several animations that are compatible with the skeleton
#pragma once

namespace pa
{
	class SceneComponent;
	class SkeletonComponent;
	class AnimationComponent;
	class Character
	{
	public:
		Character(ID3D11Device* device);
		Character(Character& other) = delete;
		Character(Character&& other) noexcept;
		~Character();

		Character& operator=(Character const& other) = delete;
		Character& operator=(Character&& other) noexcept;

	public:
		void update(float deltaTime, ID3D11DeviceContext* deviceContext);
		void processInput(float deltaTime);
		void setPosition(float x, float y, float z);

	public:
		float								_moveTime				= 0.0f;
		float								_jumpTime				= 0.0f;
		float								_attackTime				= 0.0f;
		bool								_isMoving				= false;
		bool								_isJumping				= false;
		bool								_isAttacking			= false;

	public:
		SkeletonComponent*					_skeletonComp;
		std::shared_ptr<SceneComponent>		_sceneComp;
		std::shared_ptr<AnimationComponent>	_animationComp;
	};
}


