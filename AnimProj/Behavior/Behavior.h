// author: Changwan Yu
#pragma once

namespace pa
{
	class Behavior
	{

	public:
		class Node abstract
		{
		public:
			Node() = default;
			virtual ~Node() = default;
			virtual bool run() abstract;
		};

		class Composite abstract : public Node
		{
		public:
			void addChild(std::shared_ptr<Node> child);

		protected:
			std::vector<std::shared_ptr<Node>> _children;
		};

		class Sequence final : public Composite
		{
		public:
			virtual bool run() override;
		};

		class Selector final : public Composite
		{
		public:
			virtual bool run() override;
		};

	public:
		Behavior() = delete;
		~Behavior() = delete;
	};

}


