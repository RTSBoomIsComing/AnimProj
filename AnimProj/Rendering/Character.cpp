#include "pch.h"
#include "Character.h"
#include "../Animation/AnimationManager.h"

#include "../Rendering/Mesh.h"
#include "../Rendering/StickMesh.h"
#include "../Rendering/CubeMesh.h"
#include "../Keyboard.h"

pa::Character::Character(ID3D11Device* device)
{
	_skeleton = &AnimationManager::get().getDefaultSkeleton();

	_jointTransforms.resize(_skeleton->getBoneCount());
	_boneStickTransforms.resize(_skeleton->getBoneCount());
	
	//_poseScales.resize(_skeleton->getBoneCount());
	_poseRotations.resize(_skeleton->getBoneCount());
	_poseTranslations.resize(_skeleton->getBoneCount());

	
	_boneMesh = new StickMesh(device);
	_jointMesh = new CubeMesh(device, 0.25f);


	for (const auto& anim : AnimationManager::get().getAnimationList())
	{
		_animationPlayers.push_back(AnimationPlayer(anim));
	}
}

pa::Character::~Character()
{

	if (nullptr != _boneMesh)
	{
		delete _boneMesh;
		_boneMesh = nullptr;
	}

	if (nullptr != _jointMesh)
	{
		delete _jointMesh;
		_jointMesh = nullptr;
	}
}

void pa::Character::update(float deltaTime, ID3D11DeviceContext* deviceContext)
{
	for (auto& animationPlayer : _animationPlayers)
	{
		animationPlayer.update(deltaTime);
	}

	this->controlAnimationPlayers();

	using namespace DirectX;
	for (const size_t boneIndex : _skeleton->getHierarchy())
	{
		XMVECTOR animationRotation;
		if (_skeleton->getUpperBodyMask()[boneIndex])
			animationRotation = _animationPlayers[2].getBoneRotation(boneIndex);
		else
			animationRotation = _animationPlayers[1].getBoneRotation(boneIndex);


		XMMATRIX animationMatrix = XMMatrixRotationQuaternion(XMQuaternionNormalize(animationRotation));

		const size_t parentBoneIndex			= _skeleton->getParentBoneID(boneIndex);
		const XMMATRIX& parentWorldTransform	= (boneIndex != 0) ?
			XMLoadFloat4x4(&_jointTransforms[parentBoneIndex]) : XMMatrixIdentity();

		XMMATRIX boneMatrix = _skeleton->getBoneMatrix(boneIndex);

		XMVECTOR boneTranslation		= {};
		XMVECTOR dummyVector			= {};
		XMMatrixDecompose(&dummyVector, &dummyVector, &boneTranslation, boneMatrix);

		XMStoreFloat4x4(&_jointTransforms[boneIndex], animationMatrix * boneMatrix * parentWorldTransform);

		const float boneStickScale = XMVectorGetX(XMVector3Length(boneTranslation));
		if (boneStickScale <= 0)
		{
			_boneStickTransforms[boneIndex] = XMFLOAT4X4{};
			continue;
		}

		const XMVECTOR V0 = XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f };
		const XMVECTOR V1 = XMVector3Normalize(boneTranslation);

		const float		dotProduct = XMVectorGetX(XMVector3Dot(V0, V1));
		const float		angle = std::acosf(dotProduct);
		const XMVECTOR	rotationAxis = XMVector3Cross(V0, V1);

		XMStoreFloat4x4(&_boneStickTransforms[boneIndex],
			XMMatrixScaling(0.15f, boneStickScale, 0.15f) * XMMatrixRotationAxis(rotationAxis, angle)
			* XMMatrixTranslation(0.f, 0.f, 0.f) * parentWorldTransform);
	}

	_boneMesh->updateInstanceData(deviceContext, _boneStickTransforms.data(), _boneStickTransforms.size());
	_jointMesh->updateInstanceData(deviceContext, _jointTransforms.data(), _jointTransforms.size());
}

void pa::Character::render(ID3D11DeviceContext* deviceContext)
{
	_boneMesh->drawInstanced(deviceContext, static_cast<UINT>(_skeleton->getBoneCount()));
	_jointMesh->drawInstanced(deviceContext, static_cast<UINT>(_skeleton->getBoneCount()));
}

void pa::Character::processInput(float deltaTime)
{

	if (Keyboard::get().getKeyState(32 /* space bar */))
	{
		for (auto& animationPlayer : _animationPlayers)
		{
			animationPlayer.play();
		}
	}

	if (Keyboard::get().getKeyState(27 /* esc */))
	{
		for (auto& animationPlayer : _animationPlayers)
		{
			animationPlayer.stop();
		}
	}

	if (Keyboard::get().getKeyState('F'))
	{
		_isAttacking = true;
	}
	else
	{
		_isAttacking = false;
	}

	if (Keyboard::get().getKeyState('W'))
	{
		_moveSpeed = std::min(1.0f, _moveSpeed += deltaTime);
	}
	else
	{
		_moveSpeed = std::max(0.0f, _moveSpeed -= deltaTime);
	}

	_isMoving = (_moveSpeed > 0.0f);
}

void pa::Character::controlAnimationPlayers()
{
	// Update pose


	if (_isMoving)
	{

	}
}
