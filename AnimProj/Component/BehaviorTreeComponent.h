// author: Changwan Yu
#pragma once
namespace pa
{
	class BehaviorTree;
	class BehaviorTreeComponent
	{
	public:
		void update(float deltaTime);
	public:
		inline void setBehaviorTree(std::shared_ptr<BehaviorTree> behaviorTree) { _behaviorTree = std::move(behaviorTree); }
	private:
		std::shared_ptr<BehaviorTree> _behaviorTree;
	};
}

