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
			float angle;
			float length;
			float mass;
		};

		enum class Channel : uint16_t
		{
			RX, RY, RZ, TX, TY, TZ, LN, None = 0
		};

	private:
		Acclaim() = delete;
		~Acclaim() = delete;
	};


	class Acclaim::Skeleton
	{
	public:
		Skeleton(const wchar_t* filePath);
		~Skeleton() = default;


	private:
		Acclaim::Unit unit;
	};
}

