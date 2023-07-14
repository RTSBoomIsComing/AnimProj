#include "pch.h"
#include "IAnimationController.h"

void pa::IAnimationController::update(float deltaTime)
{
	if (true != _isRunning)
		return;

	_runningTime += deltaTime;
}

void pa::IAnimationController::play()
{
	_isRunning = true;
}

void pa::IAnimationController::pause() 
{
	_isRunning = false;
}

void pa::IAnimationController::reset() 
{
	_runningTime = 0.0f;
}

void pa::IAnimationController::setCyclic(bool onoff) { 
	_isCyclic = onoff; 
}

bool pa::IAnimationController::isRunning() const 
{
	return _isRunning; 
}

void pa::IAnimationController::addBlendWeight(float weight)
{
	_blendWeight += weight;
	if (_blendWeight > 1.0f)
		_blendWeight = 1.0f;

	else if (_blendWeight < 0.0f)
		_blendWeight = 0.0f;
}
