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
		virtual void reset()					abstract;


		inline void play()					{ _isRunning = true;	}
		inline void pause()					{ _isRunning = false;	}
		inline void setCyclic(bool onoff)	{ _isCyclic = onoff;	}
		inline bool isRunning() const		{ return _isRunning;	}

		inline std::vector<DirectX::XMVECTOR> const& getRotations() const { return _rotations; }

	protected:
		bool								_isCyclic		= true;
		float								_runningTime	= 0.0f;
		std::vector<DirectX::XMVECTOR>		_rotations;

	private:
		bool								_isRunning		= false;
	};

}

