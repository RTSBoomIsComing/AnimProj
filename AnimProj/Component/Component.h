// author: Changwan Yu
#pragma once
namespace pa
{
	class Component
	{
	};

	template<typename ComponentType>
	class ComponentHandle
	{
	public:
		ComponentHandle(std::vector<ComponentType>& pool, size_t id)
			: _componentPool(&pool)
			, _componentID(id) {}
		ComponentType& get()
		{
			assert(_componentID < _componentPool->size());
			return (*_componentPool)[_componentID];
		}

	private:
		std::vector<ComponentType>* _componentPool;
		size_t _componentID;
	};
}

