// author: Changwan Yu
#pragma once
namespace pa
{
	class World;

	using ComponentID = size_t;
	using EntityID	  = size_t;

	template <typename T>
	class Component
	{
	protected:
		inline static std::vector<T>& getComponentContainer()
		{
			static std::vector<T> container;
			return container;
		}

		inline static std::vector<EntityID>& getOwners()
		{
			std::vector<EntityID> owners;
			return owners;
		}

		inline static std::vector<ComponentID>& getEntities()
		{
			std::vector<ComponentID> entities;
			return entities;
		}

	public:
		inline static size_t createComponent(EntityID owner)
		{
			std::vector<T>&			  container = getComponentContainer();
			std::vector<EntityID>&	  owners	= getOwners();
			std::vector<ComponentID>& entities	= getEntities();

			const ComponentID component = container.size();
			container.emplace_back();
			owners.push_back(owner);

			if (entities.size() <= owner)
				entities.resize(owner + 1);

			entities[owner] = component;

			return component;
		}

		inline static void destroyComponent(ComponentID component)
		{
			std::vector<T>&			  container = getComponentContainer();
			std::vector<EntityID>&	  owners	= getOwners();
			std::vector<ComponentID>& entities	= getEntities();

			entities[owners.back()] = component;

			std::iter_swap(container.begin() + component, container.end() - 1);
			std::iter_swap(owners.begin() + component, owners.end() - 1);

			container.pop_back();
			owners.pop_back();
		}

		inline static T* get(ComponentID component)
		{
			if (std::numeric_limits<size_t>::max() == component)
				return nullptr;

			return getComponentContainer()[component];
		}

		inline static EntityID getOwner(ComponentID component)
		{
			std::vector<EntityID>& owners = getOwners();
			return owners[component];
		}

	public:
		inline ComponentID getComponentID() const
		{
			std::vector<T>&	  container = getComponentContainer();
			const ComponentID component = std::distance(container.begin(), this);
			return component;
		}
		inline EntityID getOwnerID() const
		{
			std::vector<EntityID>& owners	 = getOwners();
			const ComponentID	   component = getComponentID();
			return owners[component];
		}

		void onUpdate(World& world, float deltaTime)
		{
			static_cast<T*>(this)->onUpdate(world, deltaTime);
		}
	};

	template <typename ComponentType>
	struct ComponentHandle
	{
		ComponentHandle() = default;
		explicit ComponentHandle(size_t id)
			: id(id) {}

		size_t id = std::numeric_limits<size_t>::max();
	};
}
