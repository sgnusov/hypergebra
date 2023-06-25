#include <set>
#include <cmath>

#include "instructions.h"
#include "object_list.h"

std::vector<std::unique_ptr<Instruction>> instructions;

Instruction::Instruction(int id) : id(id), priority(0), is_disabled(false) {}

Instruction::Instruction(int id, int priority_) : Instruction(id) {
	priority = priority_;
}

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

CreatePointOnLine::CreatePointOnLine(int id, Point p, const std::string& line_id) : Instruction(id), p(p), line_id(line_id) {}

void CreatePointOnLine::execute() {
	std::shared_ptr<Line> l = std::static_pointer_cast<Line>(objects[line_id]);
	if(l == nullptr)
		return;
	objects.add(std::to_string(id), id, std::make_shared<PointOnLine>(p, *l));
}

std::string CreatePointOnLine::getLineId() const {
	return line_id;
}

IntersectLines::IntersectLines(int id, const std::string& idp, const std::string& idq) : Instruction(id), idp(idp), idq(idq) {}

void IntersectLines::execute() {
	std::shared_ptr<Line> p = std::static_pointer_cast<Line>(objects[idp]);
	std::shared_ptr<Line> q = std::static_pointer_cast<Line>(objects[idq]);
	if(p == nullptr || q == nullptr) {
		//objects.add(std::to_string(id), id, nullptr);
		return;
	}
	Point res = (*p) ^ (*q);
	if(res.sq() >= 0) {
		return;
	}
	res = res / std::sqrt(-res.sq());
	if(res[0] + res[1] + res[2] < 0) {
		res = -res;
	}
	objects.add(std::to_string(id), id, std::make_shared<FixedPoint>(res));
}

DeleteObject::DeleteObject(int id, int instruction_id) : Instruction(id, -1), instruction_id(instruction_id) {}

void DeleteObject::execute() {
	instructions[instruction_id]->disable();
}
