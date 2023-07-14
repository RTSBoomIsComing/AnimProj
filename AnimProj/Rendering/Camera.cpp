// author: Changwan Yu
#include "pch.h"
#include "Camera.h"
#include "../Keyboard.h"

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

	using namespace DirectX;
	XMStoreFloat4(&_eyePosition, XMVECTOR{ 0.0f, 0.0f, -10.0f, 1.0f });
	XMStoreFloat4(&_eyeRotation, XMVECTOR{ 0.0f, 0.0f,   0.0f, 1.0f });
	XMStoreFloat4(&_focusPosition, XMVECTOR{ 0.0f, 0.0f,  0.0f, 1.0f });

	calculateMatrices();
}


void pa::Camera::setAspectRatio(float aspectRatio)
{
	_aspectRatio = aspectRatio;
}

void pa::Camera::setEyePosition(const DirectX::XMVECTOR& eyePosition)
{
	DirectX::XMStoreFloat4(&_eyePosition, eyePosition);
}

void pa::Camera::setFocusPosition(const DirectX::XMVECTOR& focusPosition)
{
	DirectX::XMStoreFloat4(&_focusPosition, focusPosition);
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
	// need to move to Camera class
	// arrow keys
	// left  : 37
	// up    : 38
	// right : 39
	// down  : 40
	float rotationY = 0.0f;
	float rotationX = 0.0f;
	if (GKeyboard->keyState[37])
		rotationX -= deltaTime;
	if (GKeyboard->keyState[39])
		rotationX += deltaTime;
	if (GKeyboard->keyState[38])
		rotationY += deltaTime;
	if (GKeyboard->keyState[40])
		rotationY -= deltaTime;

	XMVECTOR quaternion = XMQuaternionRotationNormal({ 1.0f, 0.0f, 0.0f, 0.0f }, rotationY * 3.0f);
	quaternion = XMQuaternionMultiply(quaternion, XMQuaternionRotationNormal({ 0.0f, 1.0f, 0.0f, 0.0f }, rotationX * 3.0f));

	//XMVECTOR currentEyeRotation = XMLoadFloat4(&_eyeRotation);
	//currentEyeRotation = XMQuaternionMultiply(currentEyeRotation, quaternion);
	//XMStoreFloat4(&_eyeRotation, currentEyeRotation);

	XMVECTOR currentEyePosition = XMLoadFloat4(&_eyePosition);
	XMStoreFloat4(&_eyePosition, XMVector4Transform(currentEyePosition, XMMatrixRotationQuaternion(quaternion)));
}

void pa::Camera::calculateMatrices()
{
	using namespace DirectX;

	const XMVECTOR upDirection{ 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&_matrices.View,
		XMMatrixLookAtLH(XMLoadFloat4(&_eyePosition), XMLoadFloat4(&_focusPosition), upDirection));

	constexpr float nearZ = 0.01f;
	constexpr float farZ = 100.0f;
	XMStoreFloat4x4(&_matrices.Projection,
		XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, _aspectRatio, nearZ, farZ));
}

const pa::Camera::Matrices& pa::Camera::getMatrices(void)
{
	return _matrices;
}
