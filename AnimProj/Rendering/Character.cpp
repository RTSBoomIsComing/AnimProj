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
	
	_poseCache[0].resize(_skeleton->getBoneCount());
	_poseCache[1].resize(_skeleton->getBoneCount());


	_boneMesh = new StickMesh(device);
	_jointMesh = new CubeMesh(device, 0.25f);

	for (const auto& anim : AnimationManager::get().getAnimationList())
	{
		_animationPlayers.push_back(AnimationPlayer(anim));
	}

	this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).setLoop(false);
	this->getAnimationPlayer(AnimPlayerIndex::Jump_up).setLoop(false);
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
	using namespace DirectX;

	for (auto& animationPlayer : _animationPlayers)
	{
		animationPlayer.update(deltaTime);
	}

	this->updatePose();

	for (const size_t boneID : _skeleton->getHierarchy())
	{
		XMVECTOR S = XMLoadFloat4(&_poseCache[0][boneID].scale);
		XMVECTOR R = XMLoadFloat4(&_poseCache[0][boneID].rotation);
		XMVECTOR T = XMLoadFloat4(&_poseCache[0][boneID].translation);

		XMMATRIX animationMatrix = XMMatrixAffineTransformation(S, XMVectorZero(), R, T);

		const size_t	parentID				= _skeleton->getParentBoneID(boneID);
		const XMMATRIX& parentWorldTransform	= (boneID != 0) ?
			XMLoadFloat4x4(&_jointTransforms[parentID]) : XMMatrixIdentity();

		XMMATRIX boneMatrix = _skeleton->getBoneMatrix(boneID);

		XMVECTOR boneTranslation		= {};
		XMVECTOR dummyVector			= {};
		XMMatrixDecompose(&dummyVector, &dummyVector, &boneTranslation, boneMatrix);

		XMStoreFloat4x4(&_jointTransforms[boneID], animationMatrix * boneMatrix * parentWorldTransform);

		const float boneStickScale = XMVectorGetX(XMVector3Length(boneTranslation));
		if (boneStickScale <= 0)
		{
			_boneStickTransforms[boneID] = XMFLOAT4X4{};
			continue;
		}

		const XMVECTOR V0 = XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f };
		const XMVECTOR V1 = XMVector3Normalize(boneTranslation);

		const float		dotProduct = XMVectorGetX(XMVector3Dot(V0, V1));
		const float		angle = std::acosf(dotProduct);
		const XMVECTOR	rotationAxis = XMVector3Cross(V0, V1);

		XMStoreFloat4x4(&_boneStickTransforms[boneID],
			XMMatrixScaling(0.15f, boneStickScale, 0.15f) * XMMatrixRotationAxis(rotationAxis, angle)
			* XMMatrixTranslation(0.f, 0.f, 0.f) * parentWorldTransform);
	}

	_boneMesh->updateInstanceData(deviceContext, _boneStickTransforms.data(), static_cast<UINT>(_boneStickTransforms.size()));
	_jointMesh->updateInstanceData(deviceContext, _jointTransforms.data(), static_cast<UINT>(_jointTransforms.size()));
}

void pa::Character::render(ID3D11DeviceContext* deviceContext)
{
	_boneMesh->drawInstanced(deviceContext, static_cast<UINT>(_skeleton->getBoneCount()));
	_jointMesh->drawInstanced(deviceContext, static_cast<UINT>(_skeleton->getBoneCount()));
}

void pa::Character::processInput(float deltaTime)
{
	if (Keyboard::get().getKeyState(27 /* esc */))
	{
		for (auto& animationPlayer : _animationPlayers)
		{
			animationPlayer.stop();
		}
	}

	if (Keyboard::get().getKeyState(32 /* space bar */))
	{
		_isJumping = true;
	}

	if (_isJumping)
	{
		_jumpTime = std::min(1.0f, _jumpTime + deltaTime);
	}

	if (Keyboard::get().getKeyState('F'))
	{
		_isAttacking = true;
		_attackTime = std::min(1.0f, _attackTime + deltaTime);
	}
	else
	{
		_isAttacking = false;
		_attackTime = std::max(0.0f, _attackTime - deltaTime);
	}

	if (Keyboard::get().getKeyState('W'))
	{
		_moveTime = std::min(2.0f, _moveTime + deltaTime);
	}
	else
	{
		_moveTime = std::max(0.0f, _moveTime - deltaTime);
	}

	_isMoving = (_moveTime > 0.0f);
}

void pa::Character::updatePose()
{
	using namespace DirectX;

	if (this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).getRunningRate() >= 1.0f)
	{
		_isJumping = false;
		_jumpTime = 0.0f;
		_moveTime = 0.0f;
		_attackTime = 0.0f;
		_moveTime = 0.0f;
		this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).reset();
		this->getAnimationPlayer(AnimPlayerIndex::Jump_up).reset();
	}

	if (_isJumping)
	{
		this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).play();
		this->getAnimationPlayer(AnimPlayerIndex::Jump_up).play();
	}

	if (_isMoving)
	{
		this->getAnimationPlayer(AnimPlayerIndex::Walk_up).play();
		this->getAnimationPlayer(AnimPlayerIndex::Walk_lo).play();
		this->getAnimationPlayer(AnimPlayerIndex::Run_up).play();
		this->getAnimationPlayer(AnimPlayerIndex::Run_lo).play();
	}
	else
	{
		this->getAnimationPlayer(AnimPlayerIndex::Walk_up).stop();
		this->getAnimationPlayer(AnimPlayerIndex::Walk_lo).stop();
		this->getAnimationPlayer(AnimPlayerIndex::Run_up).stop();
		this->getAnimationPlayer(AnimPlayerIndex::Run_lo).stop();
	}
	
	if (_isAttacking)
	{
		this->getAnimationPlayer(AnimPlayerIndex::Punch_up).play();
		this->getAnimationPlayer(AnimPlayerIndex::Punch_lo).play();
	}
	else
	{
		this->getAnimationPlayer(AnimPlayerIndex::Punch_up).stop();
		this->getAnimationPlayer(AnimPlayerIndex::Punch_lo).stop();
	}

	if (!_isJumping)
	{
		this->getAnimationPlayer(AnimPlayerIndex::Jump_up).storePose(_poseCache[0]);
		this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).storePose(_poseCache[0]);
	}

	this->getAnimationPlayer(AnimPlayerIndex::Walk_up).blendPoseWithBase(_poseCache[0], _moveTime);
	this->getAnimationPlayer(AnimPlayerIndex::Walk_lo).blendPoseWithBase(_poseCache[0], _moveTime);

	this->getAnimationPlayer(AnimPlayerIndex::Run_up).blendPoseWithBase(_poseCache[0], _moveTime - 1.0f);
	this->getAnimationPlayer(AnimPlayerIndex::Run_lo).blendPoseWithBase(_poseCache[0], _moveTime - 1.0f);
	
	this->getAnimationPlayer(AnimPlayerIndex::Punch_up).blendPoseWithBase(_poseCache[0], _attackTime);
	
	this->getAnimationPlayer(AnimPlayerIndex::Jump_up).blendPoseWithBase(_poseCache[0], _jumpTime);
	this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).blendPoseWithBase(_poseCache[0], _jumpTime);

}
