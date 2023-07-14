// author: Changwan Yu
#pragma once
namespace pa
{
	class IAnimationController
	{
	public:
		IAnimationController() = default;
		virtual ~IAnimationController() = default;

		virtual void				update(float deltaTime);
		virtual DirectX::XMVECTOR	getBoneRotation(size_t boneIndex, uint32_t offset = 0) const abstract;


		void play();
		void pause();
		void reset();
		void setCyclic(bool onoff);
		bool isRunning() const;
		void addBlendWeight(float weight);

	protected:
		std::vector<DirectX::XMVECTOR>		_rotations;
		float								_runningTime	= 0.0f;
		float								_blendWeight	= 0.0f;
		uint32_t							_duration		= 0;
		bool								_isRunning		= true;
		bool								_isCyclic		= true;
	};
}

