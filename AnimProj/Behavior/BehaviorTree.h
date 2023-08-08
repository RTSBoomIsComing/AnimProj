// author: Changwan Yu
#pragma once
namespace pa
{
	class BehaviorTree
	{
	protected:
		class Node abstract
		{
		public:
			virtual bool run() abstract;
		};

		class Composite abstract : public Node
		{
		public:
			void addChild(std::shared_ptr<Node> child)
			{
				_children.push_back(std::move(child));
			}

		protected:
			std::vector<std::shared_ptr<Node>> _children;

		};

		class Sequence final : public Composite
		{
		public:
			virtual bool run() override
			{
				for (std::shared_ptr<Node> child : _children)
				{
					const bool result = child->run();
					if (false == result)
						return false;
				}

				return true;
			}
		};

		class Selector final : public Composite
		{
		public:
			virtual bool run() override
			{
				for (std::shared_ptr<Node> child : _children)
				{
					const bool result = child->run();
					if (true == result)
						return true;
				}

				return false;
			}
		};

	public:
		Node* getRoot() { return _root.get(); }
	protected:
		std::shared_ptr<Node> _root;
	};
}

