// author: Changwan Yu
#pragma once
namespace pa
{
	class Component
	{
	};

	template <typename ComponentType>
	struct ComponentHandle
	{
		ComponentHandle() = default;
		explicit ComponentHandle(size_t id) : id(id) {}

		size_t id = std::numeric_limits<size_t>::max();
	};
}

