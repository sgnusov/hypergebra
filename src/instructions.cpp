#include <set>
#include <cmath>

#include "instructions.h"
#include "object_list.h"
#include "common.h"

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


CreateCircle::CreateCircle(int id, const std::string& center_id, const std::string& point_id)
	: Instruction(id), center_id(center_id), point_id(point_id) {}

void CreateCircle::execute() {
	std::shared_ptr<Point> center = std::static_pointer_cast<Point>(objects[center_id]);
	std::shared_ptr<Point> point = std::static_pointer_cast<Point>(objects[point_id]);
	if(center == nullptr || point == nullptr) {
		//objects.add(std::to_string(id), id, nullptr);
		return;
	}
	objects.add(std::to_string(id), id, std::make_shared<Circle>(*center, *point));
}


CreatePointOnCircle::CreatePointOnCircle(int id, Point p, const std::string& circle_id) : Instruction(id), p(p), circle_id(circle_id) {}

void CreatePointOnCircle::execute() {
	std::shared_ptr<Circle> c = std::static_pointer_cast<Circle>(objects[circle_id]);
	if(c == nullptr)
		return;
	objects.add(std::to_string(id), id, std::make_shared<PointOnCircle>(p, *c));
}

std::string CreatePointOnCircle::getCircleId() const {
	return circle_id;
}


IntersectCircleLine::IntersectCircleLine(int id, const std::string& circle_id, const std::string& line_id)
	: Instruction(id), circle_id(circle_id), line_id(line_id) {}

void IntersectCircleLine::execute() {
	std::shared_ptr<Circle> circle_ptr = std::static_pointer_cast<Circle>(objects[circle_id]);
	std::shared_ptr<Line> line_ptr = std::static_pointer_cast<Line>(objects[line_id]);
	if(circle_ptr == nullptr || line_ptr == nullptr) {
		return;
	}
	Circle circle = *circle_ptr;
	Line line = *line_ptr;
	Point p = circle.getCenter();
	p = p - line * (line * p);
	p = p * (-circle.getC() / (p * circle.getCenter()));
	Point v = line ^ circle.getCenter();
	ld k = -(p * p + 1) / (v * v);
	if(lt(k, 0)) {
		return;
	}
	if(eq(k, 0)) {
		objects.add(std::to_string(id), id, std::make_shared<FixedPoint>(p));
		return;
	}
	k = std::sqrt(k);
	objects.add(std::to_string(id) + ".0", id, std::make_shared<FixedPoint>(p + v * k));
	objects.add(std::to_string(id) + ".1", id, std::make_shared<FixedPoint>(p - v * k));
}


IntersectCircles::IntersectCircles(int id, const std::string& circle1_id, const std::string& circle2_id)
	: Instruction(id), circle1_id(circle1_id), circle2_id(circle2_id) {}

void IntersectCircles::execute() {
	std::shared_ptr<Circle> circle1_ptr = std::static_pointer_cast<Circle>(objects[circle1_id]);
	std::shared_ptr<Circle> circle2_ptr = std::static_pointer_cast<Circle>(objects[circle2_id]);
	if(circle1_ptr == nullptr || circle2_ptr == nullptr) {
		return;
	}
	Circle circle1 = *circle1_ptr;
	Circle circle2 = *circle2_ptr;
	Point p1 = circle1.getCenter();
	Point p2 = circle2.getCenter();
	ld c1 = circle1.getC();
	ld c2 = circle2.getC();
	ld k = p1 * p2;
	Point p = (p1 * (c1 + c2 * k) + p2 * (c1 * k + c2)) / (1 - k * k);
	Point v = p1 ^ p2;
	k = -(p * p + 1) / (v * v);
	if(lt(k, 0)) {
		return;
	}
	if(eq(k, 0)) {
		objects.add(std::to_string(id), id, std::make_shared<FixedPoint>(p));
		return;
	}
	k = std::sqrt(k);
	objects.add(std::to_string(id) + ".0", id, std::make_shared<FixedPoint>(p + v * k));
	objects.add(std::to_string(id) + ".1", id, std::make_shared<FixedPoint>(p - v * k));
}

PerpendicularPointLine::PerpendicularPointLine(int id, const std::string& point_id, const std::string& line_id)
	: Instruction(id), point_id(point_id), line_id(line_id) {}

void PerpendicularPointLine::execute() {
	std::shared_ptr<Point> point_ptr = std::static_pointer_cast<Point>(objects[point_id]);
	std::shared_ptr<Line> line_ptr = std::static_pointer_cast<Line>(objects[line_id]);
	if(point_ptr == nullptr || line_ptr == nullptr) {
		return;
	}
	objects.add(std::to_string(id), id, std::make_shared<Line>((*point_ptr) ^ (*line_ptr)));
}

PerpendicularBisector::PerpendicularBisector(int id, const std::string& point1_id, const std::string& point2_id)
	: Instruction(id), point1_id(point1_id), point2_id(point2_id) {}

void PerpendicularBisector::execute() {
	std::shared_ptr<Point> point1_ptr = std::static_pointer_cast<Point>(objects[point1_id]);
	std::shared_ptr<Point> point2_ptr = std::static_pointer_cast<Point>(objects[point2_id]);
	if(point1_ptr == nullptr || point2_ptr == nullptr) {
		return;
	}
	objects.add(std::to_string(id), id, std::make_shared<Line>((*point1_ptr) - (*point2_ptr)));
}

MiddlePoint::MiddlePoint(int id, const std::string& point1_id, const std::string& point2_id)
	: Instruction(id), point1_id(point1_id), point2_id(point2_id) {}

void MiddlePoint::execute() {
	std::shared_ptr<Point> point1_ptr = std::static_pointer_cast<Point>(objects[point1_id]);
	std::shared_ptr<Point> point2_ptr = std::static_pointer_cast<Point>(objects[point2_id]);
	if(point1_ptr == nullptr || point2_ptr == nullptr) {
		return;
	}
	Point res = (*point1_ptr) + (*point2_ptr);
	res = res / 2;
	res = res / std::sqrt(-res.sq());
	objects.add(std::to_string(id), id, std::make_shared<FixedPoint>(res));
}

AngleBisector::AngleBisector(int id, const std::string& point1_id, const std::string& point2_id, const std::string& point3_id)
	: Instruction(id), point1_id(point1_id), point2_id(point2_id), point3_id(point3_id) {}

void AngleBisector::execute() {
	std::shared_ptr<Point> point1_ptr = std::static_pointer_cast<Point>(objects[point1_id]);
	std::shared_ptr<Point> point2_ptr = std::static_pointer_cast<Point>(objects[point2_id]);
	std::shared_ptr<Point> point3_ptr = std::static_pointer_cast<Point>(objects[point3_id]);
	if(point1_ptr == nullptr || point2_ptr == nullptr || point3_ptr == nullptr) {
		return;
	}
	Point point1 = *point1_ptr;
	Point point2 = *point2_ptr;
	Point point3 = *point3_ptr;
	Point v1 = -point1 / (point1 * point2) - point2;
	Point v3 = -point3 / (point3 * point2) - point2;
	v1 = v1 / std::sqrt(v1.sq());
	v3 = v3 / std::sqrt(v3.sq());
	objects.add(std::to_string(id), id, std::make_shared<Line>((v1 + v3) ^ point2));
}

PolarLine::PolarLine(int id, const std::string& circle_id, const std::string& point_id)
	: Instruction(id), circle_id(circle_id), point_id(point_id) {}

void PolarLine::execute() {
	std::shared_ptr<Circle> circle_ptr = std::static_pointer_cast<Circle>(objects[circle_id]);
	std::shared_ptr<Point> point_ptr = std::static_pointer_cast<Point>(objects[point_id]);
	if(circle_ptr == nullptr || point_ptr == nullptr) {
		return;
	}
	Circle circle = *circle_ptr;
	Point point = *point_ptr;
	Point center = circle.getCenter();
	ld c = circle.getC();
	Point p = center * (center * point) + point * c * c;
	if(gt(p.sq(), 0)) {
		objects.add(std::to_string(id), id, std::make_shared<Line>(center * (center * point) + point * c * c));
	}
}

Tangents::Tangents(int id, const std::string& circle_id, const std::string& point_id)
	: Instruction(id), circle_id(circle_id), point_id(point_id) {}

void Tangents::execute() {
	std::shared_ptr<Circle> circle_ptr = std::static_pointer_cast<Circle>(objects[circle_id]);
	std::shared_ptr<Point> point_ptr = std::static_pointer_cast<Point>(objects[point_id]);
	if(circle_ptr == nullptr || point_ptr == nullptr) {
		return;
	}
	Circle circle = *circle_ptr;
	Point point = *point_ptr;
	Point center = circle.getCenter();
	ld c = circle.getC();
	Point p = center * (center * point) + point * c * c;
	if(!gt(p.sq(), 0))
		return;
	Line line = p;
	p = circle.getCenter();
	p = p - line * (line * p);
	p = p * (-circle.getC() / (p * center));
	Point v = line ^ center;
	ld k = -(p * p + 1) / (v * v);
	if(lt(k, 0)) {
		return;
	}
	if(eq(k, 0)) {
		objects.add(std::to_string(id), id, std::make_shared<Line>(point ^ (point ^ center)));
		return;
	}
	k = std::sqrt(k);
	objects.add(std::to_string(id) + ".0", id, std::make_shared<Line>(point, p + v * k));
	objects.add(std::to_string(id) + ".1", id, std::make_shared<Line>(point, p - v * k));
}

CircleByPoints::CircleByPoints(int id, const std::string& point1_id, const std::string& point2_id, const std::string& point3_id)
	: Instruction(id), point1_id(point1_id), point2_id(point2_id), point3_id(point3_id) {}

void CircleByPoints::execute() {
	std::shared_ptr<Point> point1_ptr = std::static_pointer_cast<Point>(objects[point1_id]);
	std::shared_ptr<Point> point2_ptr = std::static_pointer_cast<Point>(objects[point2_id]);
	std::shared_ptr<Point> point3_ptr = std::static_pointer_cast<Point>(objects[point3_id]);
	if(point1_ptr == nullptr || point2_ptr == nullptr || point3_ptr == nullptr) {
		return;
	}
	Point point1 = *point1_ptr;
	Point point2 = *point2_ptr;
	Point point3 = *point3_ptr;
	Point center = (point1 - point2) ^ (point3 - point2);
	if(!lt(center.sq(), 0))
		return;
	center = center.norm();
	objects.add(std::to_string(id), id, std::make_shared<Circle>(center, point1));
}

ReflectObject::ReflectObject(int id, const std::string& object_id, const std::string& line_id)
	: Instruction(id), object_id(object_id), line_id(line_id) {}

void ReflectObject::execute() {
	std::shared_ptr<Line> line_ptr = std::static_pointer_cast<Line>(objects[line_id]);
	std::shared_ptr<GeometryObject> object_ptr = std::static_pointer_cast<GeometryObject>(objects[object_id]);
	if(line_ptr == nullptr || object_ptr == nullptr) {
		return;
	}
	Line line = *line_ptr;
	if(object_ptr->getCapability() & POINT_CAP) {
		Point p = *std::static_pointer_cast<Point>(object_ptr);
		objects.add(std::to_string(id), id, std::make_shared<FixedPoint>(line * (-2 * (p * line)) + p));
	} else if(object_ptr->getCapability() & LINE_CAP) {
		Line l = *std::static_pointer_cast<Line>(object_ptr);
		Point res = line * (-2 * (l * line)) + l;
		objects.add(std::to_string(id), id, std::make_shared<Line>(res));
	} else if(object_ptr->getCapability() & CIRCLE_CAP) {
		Circle circle = *std::static_pointer_cast<Circle>(object_ptr);
		Point center = circle.getCenter();
		objects.add(std::to_string(id), id, std::make_shared<Circle>(line * (-2 * (center * line)) + center, circle.getC()));
	}
}

CircleCenter::CircleCenter(int id, const std::string& circle_id) : Instruction(id), circle_id(circle_id) {}

void CircleCenter::execute() {
	std::shared_ptr<Circle> circle_ptr = std::static_pointer_cast<Circle>(objects[circle_id]);
	if(circle_ptr == nullptr) {
		return;
	}
	objects.add(std::to_string(id), id, std::make_shared<FixedPoint>(circle_ptr->getCenter()));
}
