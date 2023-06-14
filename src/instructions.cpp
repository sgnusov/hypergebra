#include <set>

#include "instructions.h"
#include "object_list.h"

std::vector<std::unique_ptr<Instruction>> instructions;

Instruction::Instruction(int id) : id(id) {}

void Instruction::process() {
	if(!is_disabled)
		execute();
	is_disabled = false;
}

int Instruction::getPriority() {
	return priority;
}

int Instruction::getId() {
	return id;
}

void Instruction::disable() {
	is_disabled = true;
}

void processInstructions() {
	objects.clear();
	std::set<int> priorities;

	for(std::unique_ptr<Instruction>& instruction_ptr : instructions) {
		priorities.insert(instruction_ptr->getPriority());
	}

	for(int priority : priorities) {
		for(std::unique_ptr<Instruction>& instruction_ptr : instructions) {
			if(instruction_ptr->getPriority() == priority) {
				instruction_ptr->process();
			}
		}
	}
}


CreatePoint::CreatePoint(int id, Point p) : Instruction(id), p(p) {}

void CreatePoint::execute() {
	objects.add(std::to_string(id), id, std::make_shared<Point>(p));
}

CreateLine::CreateLine(int id, const std::string& idp, const std::string& idq) : Instruction(id), idp(idp), idq(idq) {}

void CreateLine::execute() {
	std::shared_ptr<Point> p = std::static_pointer_cast<Point>(objects[idp]);
	std::shared_ptr<Point> q = std::static_pointer_cast<Point>(objects[idq]);
	if(p == nullptr || q == nullptr) {
		//objects.add(std::to_string(id), id, nullptr);
		return;
	}
	objects.add(std::to_string(id), id, std::make_shared<Line>(*p, *q));
}
