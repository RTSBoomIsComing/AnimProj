#pragma once
namespace pa
{
	class Timer
	{
	public:
		Timer();
		~Timer() = default;

		void			update();
		inline float	getDeltaTime() const { return _deltaTime; }

	private:
		std::chrono::steady_clock::time_point	_lastTimePoint;
		float									_deltaTime		= 0.0f;
	};
}

