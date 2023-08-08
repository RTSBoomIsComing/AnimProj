#include "pch.h"
#include "CharacterBehaviorTree.h"
#include "../Rendering/Character.h"

pa::CharacterBehaviorTree::PrintID::PrintID(Character& character) : _character(&character) {}

bool pa::CharacterBehaviorTree::PrintID::run()
{
	//std::cout << _character->id << std::endl;
	return true;
}

pa::CharacterBehaviorTree::CharacterBehaviorTree(Character& character)
{
	std::shared_ptr<Behavior::Sequence> rootSequence = std::make_shared<Behavior::Sequence>();
	_root = rootSequence;

	rootSequence->addChild(std::make_shared<PrintID>(character));
	rootSequence->addChild(std::make_shared<PrintID>(character));
	rootSequence->addChild(std::make_shared<PrintID>(character));
}
