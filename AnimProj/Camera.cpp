// author: Changwan Yu
#include "pch.h"
#include "Camera.h"

pa::Camera::Camera()
{
	initialize();
}

void pa::Camera::initialize(void)
{
	constexpr float aspectRatio = 1280.0f / 720.0f;
	_aspectRatio = aspectRatio;

	using namespace DirectX;
	XMStoreFloat4(&_eyePosition, XMVECTOR{ 0.0f, 0.0f, -1.0f, 1.0f });
	XMStoreFloat4(&_focusPosition, XMVECTOR{ 0.0f, 0.0f,  0.0f, 1.0f });

	calculateMatrices();
}


void pa::Camera::updateAspectRatio(float aspectRatio)
{
	_aspectRatio = aspectRatio;
	_dirtyBit = true;
}

void pa::Camera::updateEyePosition(const DirectX::XMVECTOR& eyePosition)
{
	DirectX::XMStoreFloat4(&_eyePosition, eyePosition);
	_dirtyBit = true;
}

void pa::Camera::updateFocusPosition(const DirectX::XMVECTOR& focusPosition)
{
	DirectX::XMStoreFloat4(&_focusPosition, focusPosition);
	_dirtyBit = true;
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

	_dirtyBit = false;
}

const DirectX::XMFLOAT4& pa::Camera::getEyePosition() const
{
	return _eyePosition;
}

const DirectX::XMFLOAT4& pa::Camera::getFocusPosition() const
{
	return _focusPosition;
}

const pa::Camera::Matrices& pa::Camera::getMatrices(void)
{
	calculateMatrices();
	return _matrices;
}
