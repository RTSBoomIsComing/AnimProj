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
	
	_basePoses.resize(_skeleton->getBoneCount());
	_blendPoses.resize(_skeleton->getBoneCount());
	_resultPoses.resize(_skeleton->getBoneCount());


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
	using namespace DirectX;

	for (auto& animationPlayer : _animationPlayers)
	{
		animationPlayer.update(deltaTime);
	}

	this->updatePoses();

	for (const size_t boneID : _skeleton->getHierarchy())
	{
		XMVECTOR S = XMLoadFloat4(&_resultPoses[boneID].scale);
		XMVECTOR R = XMLoadFloat4(&_resultPoses[boneID].rotation);
		XMVECTOR T = XMLoadFloat4(&_resultPoses[boneID].translation);

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

void pa::Character::updatePoses()
{
	using namespace DirectX;

	// temporarily set these static
	static AnimPlayerIndex baseAnimUp = {};
	static AnimPlayerIndex baseAnimLo = {};

	static AnimPlayerIndex targetAnimUp = {};
	static AnimPlayerIndex targetAnimLo = {};

	if (_isJumping && 0 == _jumpTime)
	{
		targetAnimUp = AnimPlayerIndex::Jump_up;
		targetAnimLo = AnimPlayerIndex::Jump_lo;
	}

	if (_isAttacking && 0 == _attackTime)
	{
		targetAnimUp = AnimPlayerIndex::Punch_up;
		targetAnimLo = AnimPlayerIndex::Punch_lo;
	}

	if (_isMoving && 0 == _moveTime)
	{
		targetAnimLo = AnimPlayerIndex::Walk_lo;
	}

	

	this->getAnimationPlayer(baseAnimUp).storePoses(_basePoses);
	this->getAnimationPlayer(baseAnimLo).storePoses(_basePoses);

	this->getAnimationPlayer(targetAnimUp).storePoses(_blendPoses);
	this->getAnimationPlayer(targetAnimLo).storePoses(_blendPoses);

	//// Update poses
	//for (const size_t boneID : _skeleton->getHierarchy())
	//{
	//	XMVECTOR Q;
	//	if (_skeleton->isInUpperBody(boneID))

	//		Q = this->getAnimationPlayer(AnimPlayerIndex::Walk_up).getBoneRotation(boneID);
	//	else
	//		Q = this->getAnimationPlayer(AnimPlayerIndex::Walk_lo).getBoneRotation(boneID);

	//	XMStoreFloat4(&_poseRotations[boneID], Q);
	//}
}
