// author: Changwan Yu
#include "pch.h"
#include "Camera.h"
#include <DirectXTK/Keyboard.h>

pa::Camera::Camera(ID3D11Device* device)
{
	{
		// Create camera constant buffer
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(pa::Camera::Matrices);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		checkResult(device->CreateBuffer(&bufferDesc, nullptr, &_cameraConstantBuffer));
	}

	initialize();
}

void pa::Camera::initialize()
{
	constexpr float aspectRatio = 1280.0f / 720.0f;
	_aspectRatio = aspectRatio;

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

void pa::Camera::setFocusPosition(const DirectX::XMVECTOR& focusPosition)
{
	DirectX::XMStoreFloat3(&_focusPosition, focusPosition);
}

void pa::Camera::update(ID3D11DeviceContext* deviceContext)
{
	calculateMatrices();
	deviceContext->UpdateSubresource(_cameraConstantBuffer.Get(), 0, nullptr, &_matrices, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, _cameraConstantBuffer.GetAddressOf());
}

void pa::Camera::processInput(float deltaTime)
{
	using namespace DirectX;

	float deltaPitch = 0.0f;
	float deltaYaw = 0.0f;

	auto kb = DirectX::Keyboard::Get().GetState();
	
	if (kb.Left)
		deltaYaw -= deltaTime;
	if (kb.Right)
		deltaYaw += deltaTime;
	if (kb.Up)
		deltaPitch += deltaTime;
	if (kb.Down)
		deltaPitch -= deltaTime;

	_eyeYaw += deltaYaw * 3.0f;
	_eyePitch += deltaPitch * 3.0f;
	_eyePitch = std::min(_eyePitch, DirectX::XM_PIDIV2 - 0.00001f);
	_eyePitch = std::max(_eyePitch, -DirectX::XM_PIDIV2 + 0.00001f);


	const XMVECTOR Q = XMQuaternionRotationRollPitchYaw(_eyePitch, _eyeYaw, 0.0f);
	XMVECTOR V = { 0.0f, 0.0f, -_eyeDistance };
	V = XMVector3Rotate(V, Q);
	XMStoreFloat3(&_eyePosition, V);
}

void pa::Camera::calculateMatrices()
{
	using namespace DirectX;

	const XMVECTOR upDirection{ 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&_matrices.View,
		XMMatrixLookAtLH(XMLoadFloat3(&_eyePosition), XMLoadFloat3(&_focusPosition), upDirection));

	constexpr float nearZ = 0.01f;
	constexpr float farZ = 100.0f;
	XMStoreFloat4x4(&_matrices.Projection,
		XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, _aspectRatio, nearZ, farZ));
}

const pa::Camera::Matrices& pa::Camera::getMatrices(void)
{
	return _matrices;
}
