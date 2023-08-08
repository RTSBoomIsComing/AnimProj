// author: Changwan Yu
#pragma once
#include "BehaviorTree.h"
namespace pa
{
	class Character;
	class CharacterBehaviorTree : public BehaviorTree
	{
		class PrintID : public Behavior::Node
		{
		public:
			PrintID(Character& character);

		public:
			virtual bool run() override;

		private:
			Character* _character;
		};
	public:
		CharacterBehaviorTree(Character& character);
	};

}

