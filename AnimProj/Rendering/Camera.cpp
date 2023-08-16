// author: Changwan Yu
#include "pch.h"
#include "Camera.h"
#include <DirectXTK/Keyboard.h>

pa::Camera::Camera(ID3D11Device* device)
{
	{
		// Create camera constant buffer
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage		  = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth	  = sizeof(pa::Camera::Matrices);
		bufferDesc.BindFlags	  = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags	  = 0;
		checkResult(device->CreateBuffer(&bufferDesc, nullptr, &_cameraConstantBuffer));
	}

	initialize();
}

void pa::Camera::initialize()
{
	constexpr float aspectRatio = 1280.0f / 720.0f;
	_aspectRatio				= aspectRatio;

	calculateMatrices();
}


void pa::Camera::setAspectRatio(float aspectRatio)
{
	_aspectRatio = aspectRatio;
}

void pa::Camera::setEyePosition(const DirectX::XMVECTOR& eyePosition)
{
	DirectX::XMStoreFloat3(&_eyePosition, eyePosition);
}


void pa::Camera::update(ID3D11DeviceContext* deviceContext)
{
	calculateMatrices();
	constructFrustum();
	deviceContext->UpdateSubresource(_cameraConstantBuffer.Get(), 0, nullptr, &_matrices, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, _cameraConstantBuffer.GetAddressOf());
}

void pa::Camera::processInput(float deltaTime)
{
	using namespace DirectX;

	float deltaPitch	   = 0.0f;
	float deltaYaw		   = 0.0f;
	float deltaMoveForward = 0.0f;
	float deltaMoveRight   = 0.0f;
	float deltaMoveUp	   = 0.0f;
	auto  kb			   = DirectX::Keyboard::Get().GetState();

	if (kb.Left)
		deltaYaw -= deltaTime;
	if (kb.Right)
		deltaYaw += deltaTime;
	if (kb.Up)
		deltaPitch -= deltaTime;
	if (kb.Down)
		deltaPitch += deltaTime;

	if (kb.W)
		deltaMoveForward += deltaTime;
	if (kb.S)
		deltaMoveForward -= deltaTime;
	if (kb.A)
		deltaMoveRight -= deltaTime;
	if (kb.D)
		deltaMoveRight += deltaTime;
	if (kb.C)
		deltaMoveUp -= deltaTime;
	if (kb.Space)
		deltaMoveUp += deltaTime;

	XMVECTOR V		 = XMLoadFloat3(&_eyePosition);
	XMVECTOR Forward = XMLoadFloat3(&_eyeDirection);
	Forward			 = XMVectorSetY(Forward, 0.0f);
	Forward			 = XMVector3Normalize(Forward);
	XMVECTOR Right	 = XMLoadFloat3(&_rightDirection);

	V = V + Forward * deltaMoveForward * 15.0f;
	V = V + Right * deltaMoveRight * 15.0f;
	V = V + XMVectorSet(0.0f, deltaMoveUp * 15.0f, 0.0f, 0.0f);
	XMStoreFloat3(&_eyePosition, V);

	_eyeYaw += deltaYaw * 3.0f;
	_eyePitch += deltaPitch * 3.0f;
	_eyePitch = std::min(_eyePitch, DirectX::XM_PIDIV2 - 0.00001f);
	_eyePitch = std::max(_eyePitch, -DirectX::XM_PIDIV2 + 0.00001f);
}

void pa::Camera::calculateMatrices()
{
	using namespace DirectX;

	const XMVECTOR Q = XMQuaternionRotationRollPitchYaw(_eyePitch, _eyeYaw, 0.0f);
	// rotate upDirection by eyeYaw
	XMVECTOR upDirection{0.0f, 1.0f, 0.0f, 0.0f};
	upDirection = XMVector3Rotate(upDirection, Q);
	XMStoreFloat3(&_upDirection, upDirection);

	// rotate eyeDirection by eyeYaw and eyePitch
	XMVECTOR eyeDirection{0.0f, 0.0f, 1.0f, 0.0f};
	eyeDirection = XMVector3Rotate(eyeDirection, Q);
	XMStoreFloat3(&_eyeDirection, eyeDirection);

	// rotate rightDirection by eyeYaw
	XMVECTOR rightDirection{1.0f, 0.0f, 0.0f, 0.0f};
	rightDirection = XMVector3Rotate(rightDirection, Q);
	XMStoreFloat3(&_rightDirection, rightDirection);

	XMStoreFloat4x4(&_matrices.View,
					XMMatrixLookToLH(XMLoadFloat3(&_eyePosition), eyeDirection, upDirection));
	// XMStoreFloat4x4(&_matrices.View,
	//	XMMatrixLookAtLH(XMLoadFloat3(&_eyePosition), XMLoadFloat3(&_focusPosition), upDirection));

	XMStoreFloat4x4(&_matrices.Projection,
					XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, _aspectRatio, _nearZ, _farZ));
}

void pa::Camera::constructFrustum()
{
	using namespace DirectX;

	XMFLOAT4X4 newProjMat = _matrices.Projection;
	float	   zMinimum, r;

	// Calculate the minimum Z distance in the frustum.
	zMinimum	   = -newProjMat._43 / newProjMat._33;
	r			   = _farZ / (_farZ - zMinimum);
	newProjMat._33 = r;
	newProjMat._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	XMMATRIX   M = XMLoadFloat4x4(&_matrices.View) * XMLoadFloat4x4(&newProjMat);
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, M);

	// Calculate near plane of frustum.
	_frustum[0].x = matrix._14 + matrix._13;
	_frustum[0].y = matrix._24 + matrix._23;
	_frustum[0].z = matrix._34 + matrix._33;
	_frustum[0].w = matrix._44 + matrix._43;
	XMVECTOR P	  = XMLoadFloat4(&_frustum[0]);
	P			  = XMPlaneNormalize(P);
	XMStoreFloat4(&_frustum[0], P);

	// Calculate far plane of frustum.
	_frustum[1].x = matrix._14 - matrix._13;
	_frustum[1].y = matrix._24 - matrix._23;
	_frustum[1].z = matrix._34 - matrix._33;
	_frustum[1].w = matrix._44 - matrix._43;
	P			  = XMLoadFloat4(&_frustum[1]);
	P			  = XMPlaneNormalize(P);
	XMStoreFloat4(&_frustum[1], P);

	// Calculate left plane of frustum.
	_frustum[2].x = matrix._14 + matrix._11;
	_frustum[2].y = matrix._24 + matrix._21;
	_frustum[2].z = matrix._34 + matrix._31;
	_frustum[2].w = matrix._44 + matrix._41;
	P			  = XMLoadFloat4(&_frustum[2]);
	P			  = XMPlaneNormalize(P);
	XMStoreFloat4(&_frustum[2], P);

	// Calculate right plane of frustum.
	_frustum[3].x = matrix._14 - matrix._11;
	_frustum[3].y = matrix._24 - matrix._21;
	_frustum[3].z = matrix._34 - matrix._31;
	_frustum[3].w = matrix._44 - matrix._41;
	P			  = XMLoadFloat4(&_frustum[3]);
	P			  = XMPlaneNormalize(P);
	XMStoreFloat4(&_frustum[3], P);

	// Calculate top plane of frustum.
	_frustum[4].x = matrix._14 - matrix._12;
	_frustum[4].y = matrix._24 - matrix._22;
	_frustum[4].z = matrix._34 - matrix._32;
	_frustum[4].w = matrix._44 - matrix._42;
	P			  = XMLoadFloat4(&_frustum[4]);
	P			  = XMPlaneNormalize(P);
	XMStoreFloat4(&_frustum[4], P);

	// Calculate bottom plane of frustum.
	_frustum[5].x = matrix._14 + matrix._12;
	_frustum[5].y = matrix._24 + matrix._22;
	_frustum[5].z = matrix._34 + matrix._32;
	_frustum[5].w = matrix._44 + matrix._42;
	P			  = XMLoadFloat4(&_frustum[5]);
	P			  = XMPlaneNormalize(P);
	XMStoreFloat4(&_frustum[5], P);

	return;
}

const pa::Camera::Matrices& pa::Camera::getMatrices(void)
{
	return _matrices;
}

bool pa::Camera::checkFrustumWithSphere(DirectX::XMVECTOR center, float radius) const
{
	using namespace DirectX;

	// Check if the radius of the sphere is inside the view frustum.
	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&_frustum[i]), center)) < -radius)
		{
			return false;
		}
	}

	return true;
}
