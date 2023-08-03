// author: Changwan Yu
// Character has a skeleton and can hold several animations that are compatible with the skeleton
#pragma once

namespace pa
{
	class SkeletonComponent;
	class AnimationComponent;
	class Skeleton;
	class Character
	{
	public:
		Character(ID3D11Device* device);
		~Character();

	public:
		void update(float deltaTime, ID3D11DeviceContext* deviceContext);
		
		//Character clone() const;
		void processInput(float deltaTime);

	public:
		float								_moveTime				= 0.0f;
		float								_jumpTime				= 0.0f;
		float								_attackTime				= 0.0f;
		bool								_isMoving				= false;
		bool								_isJumping				= false;
		bool								_isAttacking			= false;

	private:
		const Skeleton*						_skeleton				= nullptr;

	public:
		std::shared_ptr<SkeletonComponent>	_skeletonComp;
		std::shared_ptr<AnimationComponent>	_animationComp;

		DirectX::XMFLOAT3					_worldPosition = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4					_worldRotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	};
}


