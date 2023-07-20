#pragma once
namespace pa
{
	class Acclaim
	{
	public:
		class Skeleton;
		class Motion;

	private:
		struct Unit
		{
			float angle		= 1.0f;
			float length	= 1.0f;
			float mass		= 1.0f;
		};

		enum class DOF : uint16_t
		{
			RX, RY, RZ, TX, TY, TZ, L
		};

	private:
		Acclaim() = delete;
		~Acclaim() = delete;
	};
}

