// author: Changwan Yu
#pragma once
namespace pa
{
	class IAnimationController
	{
	public:
		IAnimationController() = default;
		~IAnimationController() = default;

		virtual void update(float deltaTime)	abstract;


		inline void play()					{ _isRunning = true;	}
		inline void pause()					{ _isRunning = false;	}
		inline void reset()					{ _runningTime = 0.0f;	}
		inline void setCyclic(bool onoff)	{ _isCyclic = onoff;	}
		inline bool isRunning() const		{ return _isRunning;	}

		virtual DirectX::XMVECTOR getBoneRotation(size_t boneIndex, uint32_t offset = 0) const abstract;
	protected:
		bool								_isCyclic		= true;
		float								_runningTime	= 0.0f;
		std::vector<DirectX::XMVECTOR>		_rotations;

	private:
		bool								_isRunning		= true;
	};

}

