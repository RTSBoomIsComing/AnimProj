// author: Changwan Yu
// Character has a skeleton and can hold several animations that are compatible with the skeleton

#pragma once
#include "../Animation/Skeleton.h"
#include "../Animation/AnimationPlayer.h"
namespace pa
{
	class Mesh;
	class Character
	{
	public:
		Character(ID3D11Device* device);
		~Character();

	public:
		void update(float deltaTime, ID3D11DeviceContext* deviceContext);
		void render(ID3D11DeviceContext* deviceContext);
		void processInput(float deltaTime);

	public:
		inline DirectX::XMFLOAT3& getPosition()		{ return _position; }
		inline DirectX::XMFLOAT3& getQrientation()	{ return _orientation; }

	private:
		void controlAnimationPlayers();

	private:
		DirectX::XMFLOAT3					_position				= {};
		DirectX::XMFLOAT3					_orientation			= {};
		float								_moveSpeed				= 0.0f;
		bool								_isMoving				= false;
		bool								_isJumping				= false;
		bool								_isAttacking			= false;

	private:
		const Skeleton*						_skeleton				= nullptr;

		// will be removed
		Mesh*								_jointMesh				= nullptr;
		Mesh*								_boneMesh				= nullptr;

		enum class AnimationPlayerIndex
		{
			WALK_UPBODY, WALK_LOBODY, PUNCH_UPBODY, PUNCH_LOBODY, DANCE_UPBODY, DANCE_LOBODY
		};
		std::vector<AnimationPlayer>		_animationPlayers;

		std::vector<DirectX::XMFLOAT4X4>	_jointTransforms;
		std::vector<DirectX::XMFLOAT4X4>	_boneStickTransforms;

		std::vector<DirectX::XMVECTOR>		_poseScales;
		std::vector<DirectX::XMVECTOR>		_poseRotations;
		std::vector<DirectX::XMVECTOR>		_poseTranslations;
	};
}


