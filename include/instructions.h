#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include <memory>
#include <vector>
#include <string>

#include "geometry.h"

class Instruction;

extern std::vector<std::unique_ptr<Instruction>> instructions;

class Instruction {
	bool is_disabled = false;
	int priority = 0;
protected:
	int id;
	virtual void execute() = 0;
public:
	Instruction(int id);
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


#endif
