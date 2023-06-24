#ifndef TOOLS_H_
#define TOOLS_H_

#include <SDL.h>
#include <memory>

#include "geometry.h"

void deselectObjects();

class Tool {
public:
	virtual void processEvent(SDL_Event graphics_event) = 0;
	virtual ~Tool() = default;
};


class MovePointTool : public Tool {
	int current_instruction_id = -1;
	bool is_point_on_line;
public:
	void processEvent(SDL_Event graphics_event);
};

class CreatePointTool : public Tool {
public:
	void processEvent(SDL_Event graphics_event);
};

class CreateLineTool : public Tool {
	std::string point_id;
public:
	void processEvent(SDL_Event graphics_event);
};

class IntersectLinesTool : public Tool {
	std::string line_id;
public:
	void processEvent(SDL_Event graphics_event);
};

class HideObjectTool : public Tool {
public:
	HideObjectTool();
	~HideObjectTool();
	void processEvent(SDL_Event graphics_event);
};

#endif
