#ifndef TOOLS_H_
#define TOOLS_H_

#include <SDL.h>
#include <memory>

#include "geometry.h"

class Tool {
public:
	virtual void processEvent(SDL_Event graphics) = 0;
	virtual ~Tool() = default;
};


class MovePointTool : public Tool {
	int current_instruction_id = -1;
	bool is_point_on_line;
public:
	void processEvent(SDL_Event graphics);
};

class CreatePointTool : public Tool {
public:
	void processEvent(SDL_Event graphics);
};

class CreateLineTool : public Tool {
	std::string point_id;
public:
	void processEvent(SDL_Event graphics);
};

class IntersectLinesTool : public Tool {
	std::string line_id;
public:
	void processEvent(SDL_Event graphics);
};

#endif
