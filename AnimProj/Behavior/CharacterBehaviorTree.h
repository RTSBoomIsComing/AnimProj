// author: Changwan Yu
#pragma once
#include "BehaviorTree.h"
namespace pa
{
	class MyActor;
	class CharacterBehaviorTree : public BehaviorTree
	{
		class PrintID : public Behavior::Node
		{
		public:
			PrintID(MyActor* character);

		public:
			virtual bool run() override;

		private:
			MyActor* _character;
		};
	public:
		CharacterBehaviorTree(MyActor* character);
	};

}

