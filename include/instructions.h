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

#endif
