// author: Changwan Yu
#include "pch.h"
#include "Camera.h"


void pa::Camera::updateMatrices()
{
	using namespace DirectX;

	// TODO: Update eyePosition and focusPosition

	const XMVECTOR upDirection{ 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&_matrices.View, 
		XMMatrixLookAtLH(XMLoadFloat4(&_eyePosition), XMLoadFloat4(&_focusPosition), upDirection));

	constexpr float nearZ = 0.01f;
	constexpr float farZ = 100.0f;
	const float aspectRatio = _viewWidth / _viewHeight;
	XMStoreFloat4x4(&_matrices.Projection,
		XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, aspectRatio, nearZ, farZ));
}

const pa::Camera::Matrices& pa::Camera::getMatrices() const
{
	return _matrices;
}

void pa::Camera::initializeCamera(float viewWidth, float viewHeight)
{
	_viewWidth = viewWidth;
	_viewHeight = viewHeight;

	using namespace DirectX;
	XMStoreFloat4(&_eyePosition, XMVECTOR{ 0.0f, 0.0f, -1.0f, 1.0f });
	XMStoreFloat4(&_focusPosition, XMVECTOR{ 0.0f, 0.0f,  0.0f, 1.0f });
	
	updateMatrices();
}
