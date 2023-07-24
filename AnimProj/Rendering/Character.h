// author: Changwan Yu
// Character has a skeleton and can hold several animations that are compatible with the skeleton

#pragma once
#include "../Animation/Skeleton.h"
//#include "../Animation/IAnimationController.h"
//#include "../Animation/Animation.h"
//#include "../Animation/AnimationController.h"
#include "../Animation/AnimationPlayer.h"
namespace pa
{
	class Mesh;
	class Character
	{
	public:
		enum class State : uint16_t
		{
			Idle,
			Walk,
			Run,
			Jump,
			Attack,
		};


	public:
		Character(ID3D11Device* device);
		~Character();

	public:
		void update(float deltaTime, ID3D11DeviceContext* deviceContext);
		void render(ID3D11DeviceContext* deviceContext);
		void processInput(float deltaTime);

		//void transitState();

	private:
		Skeleton*							_skeleton				= nullptr;

		// will be removed
		Mesh*								_jointMesh				= nullptr;
		Mesh*								_boneMesh				= nullptr;

		std::vector<AnimationPlayer>		_animationPlayers;

		std::vector<DirectX::XMMATRIX>		_jointTransforms;
		std::vector<DirectX::XMMATRIX>		_boneStickTransforms;

		std::vector<DirectX::XMVECTOR>		_poseScales;
		std::vector<DirectX::XMVECTOR>		_poseRotations;
		std::vector<DirectX::XMVECTOR>		_poseTranslations;
	};
}


