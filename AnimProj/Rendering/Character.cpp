#include "pch.h"
#include "Character.h"
#include "../ASFAMC/ASF.h"
#include "../ASFAMC/AMC.h"
#include "../Animation/Animation.h"
#include "../Animation/AnimationController.h"
#include "../Animation/AnimationBlender.h"
#include "../Rendering/Mesh.h"
#include "../Rendering/StickMesh.h"
#include "../Rendering/CubeMesh.h"
#include "../Keyboard.h"

pa::Character::Character(ID3D11Device* device)
{
	std::wstring asfFilePath = _SOLUTIONDIR;
	ASF asf(asfFilePath + LR"(Assets\ASFAMC\subject02\02.asf)");

	_skeleton = asf.createSkeleton();

	std::wstring amcDirectory = _SOLUTIONDIR;
	amcDirectory += LR"(Assets\ASFAMC\subject02\)";

	AMC amcIdle(amcDirectory	+ L"idle.amc");
	AMC amcWalk(amcDirectory	+ L"walk.amc");
	AMC amcRun(amcDirectory		+ L"run_cyclic.amc");
	AMC amcJump(amcDirectory	+ L"jumpbalance.amc");
	AMC amcPunch(amcDirectory	+ L"punchstrike.amc");

	_animations.push_back(Animation(&asf, &amcIdle));
	_animations.push_back(Animation(&asf, &amcWalk));
	_animations.push_back(Animation(&asf, &amcRun));
	_animations.push_back(Animation(&asf, &amcJump));
	_animations.push_back(Animation(&asf, &amcPunch));

	_animationControllers.push_back(AnimationController(&_animations[0]));
	_animationControllers.push_back(AnimationController(&_animations[1]));
	_animationControllers.push_back(AnimationController(&_animations[2]));
	_animationControllers.push_back(AnimationController(&_animations[3]));
	_animationControllers.push_back(AnimationController(&_animations[4]));

	for (auto& animation : _animations)
	{
		animation.compressAnimation();
	}
	_animationIdleWalk	= new AnimationBlender(&_animationControllers[1], &_animationControllers[0]);
	_animationWalkRun	= new AnimationBlender(&_animationControllers[1], &_animationControllers[2]);

	_jointTransforms.resize(_skeleton->getBoneCount());
	_boneStickTransforms.resize(_skeleton->getBoneCount());


	std::vector<bool> _skeletonUpperBodyMask(_skeleton->getBoneCount());

	// bone index 11 is the lower back
	_skeletonUpperBodyMask[11] = true;
	for (uint8_t boneIndex : _skeleton->getHierarchy())
	{
		uint8_t parentIndex = _skeleton->getParentBoneIndex(boneIndex);
		if (parentIndex >= _skeleton->getBoneCount())
			continue;

		if (_skeletonUpperBodyMask[parentIndex])
			_skeletonUpperBodyMask[boneIndex] = true;
	}

	_boneMesh = new StickMesh(device);
	_jointMesh = new CubeMesh(device, 0.25f);
}

pa::Character::~Character()
{
	if (_skeleton != nullptr)
	{
		delete _skeleton;
		_skeleton = nullptr;
	}
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

	if (nullptr != _animationWalkRun)
	{
		delete _animationWalkRun;
		_animationWalkRun = nullptr;
	}

	if (nullptr != _animationIdleWalk)
	{
		delete _animationIdleWalk;
		_animationIdleWalk = nullptr;
	}
}

void pa::Character::update(float deltaTime, ID3D11DeviceContext* deviceContext)
{
	for (auto& animationController : _animationControllers)
	{
		animationController.update(deltaTime);
	}

	_animationIdleWalk->update(deltaTime);
	_animationWalkRun->update(deltaTime);

	using namespace DirectX;
	for (const size_t boneIndex : _skeleton->getHierarchy())
	{
		XMVECTOR animationRotation	= _animationIdleWalk->getBoneRotation(boneIndex);
		XMMATRIX animationMatrix	= XMMatrixRotationQuaternion(XMQuaternionNormalize(animationRotation));

		const size_t parentBoneIndex			= _skeleton->getParentBoneIndex(boneIndex);
		const XMMATRIX& parentWorldTransform	= (parentBoneIndex < _skeleton->getBoneCount()) ? 
			_jointTransforms[parentBoneIndex] : XMMatrixIdentity();

		XMMATRIX boneMatrix = _skeleton->getBoneMatrix(boneIndex);

		XMVECTOR boneTranslation		= {};
		XMVECTOR dummyVector			= {};
		XMMatrixDecompose(&dummyVector, &dummyVector, &boneTranslation, boneMatrix);


		_jointTransforms[boneIndex] = animationMatrix * boneMatrix * parentWorldTransform;

		const float boneStickScale = XMVectorGetX(XMVector3Length(boneTranslation));
		if (boneStickScale <= 0)
		{
			_boneStickTransforms[boneIndex] = XMMATRIX{};
			continue;
		}

		const XMVECTOR vec0 = XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f };
		const XMVECTOR vec1 = XMVector3Normalize(boneTranslation);

		const float		dotProduct = XMVectorGetX(XMVector3Dot(vec0, vec1));
		const float		angle = std::acosf(dotProduct);
		const XMVECTOR	rotationAxis = XMVector3Cross(vec0, vec1);


		_boneStickTransforms[boneIndex] =
			XMMatrixScaling(0.15f, boneStickScale, 0.15f) * XMMatrixRotationAxis(rotationAxis, angle)
			* XMMatrixTranslation(0.f, 0.f, 0.f) * parentWorldTransform;
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
	if (Keyboard::get()->getKeyState('W'))
	{
		_animationIdleWalk->addBlendWeight(-deltaTime * 0.5f);
		_animationWalkRun->addBlendWeight(deltaTime * 0.5f);
	}
	if (!Keyboard::get()->getKeyState('W'))
	{
		_animationIdleWalk->addBlendWeight(deltaTime * 0.5f);
		_animationWalkRun->addBlendWeight(-deltaTime * 0.5f);
	}



}
