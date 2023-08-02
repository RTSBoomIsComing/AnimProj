// author: Changwan Yu
// Character has a skeleton and can hold several animations that are compatible with the skeleton
#pragma once
#include "../Animation/AnimationPlayer.h"
namespace pa
{
	class SkeletonComponent;
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

	private:
		enum class AnimPlayerIndex
		{
			Walk_up, Walk_lo, Run_up, Run_lo, Punch_up, Punch_lo, Jump_up, Jump_lo, Dance_up, Dance_lo
		};

		AnimationPlayer& getAnimationPlayer(AnimPlayerIndex index);

		void updatePose();

	private:
		float								_moveTime				= 0.0f;
		float								_jumpTime				= 0.0f;
		float								_attackTime				= 0.0f;
		bool								_isMoving				= false;
		bool								_isJumping				= false;
		bool								_isAttacking			= false;

	private:
		const Skeleton*						_skeleton				= nullptr;


		std::vector<AnimationPlayer>		_animationPlayers;
		std::vector<Transform>				_poseCache[2];

	public:
		SkeletonComponent*					_skeletonComp			= nullptr;
		//std::vector<DirectX::XMFLOAT4X4>	_boneGTs;
		//std::vector<DirectX::XMFLOAT4X4>	_boneToBoneGTs;
		//ComPtr<ID3D11Buffer>				_boneWorldCBuffer;
		//ComPtr<ID3D11Buffer>				_boneToBoneWorldCBuffer;

		DirectX::XMFLOAT3					_worldPosition = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4					_worldRotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	};
}


