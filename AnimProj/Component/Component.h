// author: Changwan Yu
#pragma once
//namespace pa
//{
//	class World;
//
//	using ComponentID = size_t;
//	using EntityID	  = size_t;
//
//	template <typename T>
//	class Component
//	{
//	protected:
//		inline static std::vector<T>& getComponentContainer()
//		{
//			static std::vector<T> container;
//			return container;
//		}
//
//		inline static std::vector<EntityID>& getOwners()
//		{
//			static std::vector<EntityID> owners;
//			return owners;
//		}
//
//		inline static std::vector<ComponentID>& getEntities()
//		{
//			static std::vector<ComponentID> entities;
//			return entities;
//		}
//
//	public:
//		inline static size_t createComponent(EntityID ownerID)
//		{
//			std::vector<T>&			  container = getComponentContainer();
//			std::vector<EntityID>&	  owners	= getOwners();
//			std::vector<ComponentID>& entities	= getEntities();
//
//			const ComponentID component = container.size();
//			container.emplace_back();
//			owners.push_back(ownerID);
//
//			if (entities.size() <= ownerID)
//				entities.resize(ownerID + 1);
//
//			entities[ownerID] = component;
//
//			return component;
//		}
//
//		inline static void destroyComponent(ComponentID componentID)
//		{
//			std::vector<T>&			  container = getComponentContainer();
//			std::vector<EntityID>&	  owners	= getOwners();
//			std::vector<ComponentID>& entities	= getEntities();
//
//			entities[owners.back()] = componentID;
//
//			std::iter_swap(container.begin() + componentID, container.end() - 1);
//			std::iter_swap(owners.begin() + componentID, owners.end() - 1);
//
//			container.pop_back();
//			owners.pop_back();
//		}
//
//		inline static T* get(ComponentID componentID)
//		{
//			if (std::numeric_limits<size_t>::max() == componentID)
//				return nullptr;
//
//			return getComponentContainer()[componentID];
//		}
//
//		inline static EntityID getOwner(ComponentID componentID)
//		{
//			std::vector<EntityID>& owners = getOwners();
//			return owners[componentID];
//		}
//
//	public:
//		inline ComponentID getComponentID() const
//		{
//			std::vector<T>&	  container	  = getComponentContainer();
//			const ComponentID componentID = std::distance(container.begin(), this);
//			return componentID;
//		}
//		inline EntityID getOwnerID() const
//		{
//			std::vector<EntityID>& owners	   = getOwners();
//			const ComponentID	   componentID = getComponentID();
//			return owners[componentID];
//		}
//
//		void onUpdate(World& world, float deltaTime)
//		{
//			static_cast<T*>(this)->onUpdate(world, deltaTime);
//		}
//	};
//}
