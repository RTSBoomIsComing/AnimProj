// author: Changwan Yu
#pragma once
namespace pa
{
	class BehaviorTree;
	class BehaviorTreeComponent
	{
	public:
		BehaviorTreeComponent(std::shared_ptr<BehaviorTree> behaviorTree) : _behaviorTree(std::move(behaviorTree)) {}
		~BehaviorTreeComponent() = default;
	
	public:
		void update(float deltaTime);

	private:
		std::shared_ptr<BehaviorTree> _behaviorTree;
	};
}

