#include "pch.h"
#include "CharacterBehaviorTree.h"
#include "../Rendering/Character.h"


pa::CharacterBehaviorTree::CharacterBehaviorTree()
{
	std::shared_ptr<Behavior::Sequence> rootSequence = std::make_shared<Behavior::Sequence>();
	_root = rootSequence;

	//rootSequence->addChild(std::make_shared<PrintID>(actor));
	//rootSequence->addChild(std::make_shared<PrintID>(actor));
	//rootSequence->addChild(std::make_shared<PrintID>(actor));
}
