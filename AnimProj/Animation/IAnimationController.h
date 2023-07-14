// author: Changwan Yu
#pragma once
namespace pa
{
	class IAnimationController
	{
	public:
		IAnimationController() = default;
		~IAnimationController() = default;

		virtual void update(float deltaTime);


		void play();
		void pause();
		void reset();
		void setCyclic(bool onoff);
		bool isRunning() const;
		void addBlendWeight(float weight);

		virtual DirectX::XMVECTOR getBoneRotation(size_t boneIndex, uint32_t offset = 0) const;;
	protected:
		bool								_isCyclic		= true;
		float								_runningTime	= 0.0f;
		float								_blendWeight	= 0.0f;
		std::vector<DirectX::XMVECTOR>		_rotations;

	private:
		bool								_isRunning		= true;
	};

}

