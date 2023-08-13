// author: Changwan Yu
#pragma once
namespace pa
{
	class World;
	class Actor;
	class BehaviorTree;
	class BehaviorTreeComponent
	{
	public:
		void onUpdate(World& world, Actor& owner, float deltaTime);
	public:
		inline void setBehaviorTree(std::shared_ptr<BehaviorTree> behaviorTree) { _behaviorTree = std::move(behaviorTree); }
	private:
		std::shared_ptr<BehaviorTree> _behaviorTree;
	};
}

