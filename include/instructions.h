#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include <memory>
#include <vector>
#include <string>

#include "geometry.h"

class Instruction;

extern std::vector<std::unique_ptr<Instruction>> instructions;

class Instruction {
	bool is_disabled;
	int priority;
protected:
	int id;
	virtual void execute() = 0;
public:
	Instruction(int id);
	Instruction(int id, int priority);
	virtual ~Instruction() = default;

	void process();
	int getPriority();
	int getId();
	void disable();
};

void processInstructions();


class CreatePoint : public Instruction {
	void execute();
public:
	Point p;
public:
	CreatePoint(int id, Point p);
};

class CreateLine : public Instruction {
	void execute();
	std::string idp, idq;
public:
	CreateLine(int id, const std::string& idp, const std::string& idq);
};

class CreatePointOnLine : public Instruction {
	void execute();
public:
	Point p;
	std::string line_id;
public:
	CreatePointOnLine(int id, Point p, const std::string& line_id);
	std::string getLineId() const;
};

class IntersectLines : public Instruction {
	void execute();
	std::string idp, idq;
public:
	IntersectLines(int id, const std::string& idp, const std::string& idq);
};

class DeleteObject : public Instruction {
	int instruction_id;
	void execute();
public:
	DeleteObject(int id, int instruction_id);
};

class CreateCircle : public Instruction {
	void execute();
	std::string center_id, point_id;
public:
	CreateCircle(int id, const std::string& center_id, const std::string& point_id);
};

class CreatePointOnCircle : public Instruction {
	void execute();
public:
	Point p;
	std::string circle_id;
public:
	CreatePointOnCircle(int id, Point p, const std::string& circle_id);
	std::string getCircleId() const;
};

class IntersectCircleLine : public Instruction {
	void execute();
	std::string circle_id, line_id;
public:
	IntersectCircleLine(int id, const std::string& circle_id, const std::string& line_id);
};

class IntersectCircles : public Instruction {
	void execute();
	std::string circle1_id, circle2_id;
public:
	IntersectCircles(int id, const std::string& circle1_id, const std::string& circles2_id);
};

class PerpendicularPointLine : public Instruction {
	void execute();
	std::string point_id, line_id;
public:
	PerpendicularPointLine(int id, const std::string& point_id, const std::string& line_id);
};

class PerpendicularBisector : public Instruction {
	void execute();
	std::string point1_id, point2_id;
public:
	PerpendicularBisector(int id, const std::string& point1_id, const std::string& point2_id);
};

class MiddlePoint : public Instruction {
	void execute();
	std::string point1_id, point2_id;
public:
	MiddlePoint(int id, const std::string& point1_id, const std::string& point2_id);
};

class AngleBisector : public Instruction {
	void execute();
	std::string point1_id, point2_id, point3_id;
public:
	AngleBisector(int id, const std::string& point1_id, const std::string& point2_id, const std::string& point3_id);
};

class PolarLine : public Instruction {
	void execute();
	std::string circle_id, point_id;
public:
	PolarLine(int id, const std::string& circle_id, const std::string& point_id);
};

class Tangents : public Instruction {
	void execute();
	std::string circle_id, point_id;
public:
	Tangents(int id, const std::string& circle_id, const std::string& point_id);
};

class CircleByPoints : public Instruction {
	void execute();
	std::string point1_id, point2_id, point3_id;
public:
	CircleByPoints(int id, const std::string& point1_id, const std::string& point2_id, const std::string& point3_id);
};

class ReflectObject : public Instruction {
	void execute();
	std::string object_id, line_id;
public:
	ReflectObject(int id, const std::string& object_id, const std::string& line_id);
};

class CircleCenter : public Instruction {
	void execute();
	std::string circle_id;
public:
	CircleCenter(int id, const std::string& circle_id);
};

#endif
