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
	cap_t capability;
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

class IntersectObjectsTool : public Tool {
	std::string object_id;
	cap_t object_cap;
public:
	void processEvent(SDL_Event graphics_event);
};

class HideObjectTool : public Tool {
public:
	HideObjectTool();
	~HideObjectTool();
	void processEvent(SDL_Event graphics_event);
};

class DeleteObjectTool : public Tool {
public:
	void processEvent(SDL_Event graphics_event);
};

class MoveCameraTool : public Tool {
	Point last_mouse_position;
public:
	void processEvent(SDL_Event graphics_event);
};

class CreateCircleTool : public Tool {
	std::string center_id;
public:
	void processEvent(SDL_Event graphics_event);
};

class CreatePerpendicularLineTool : public Tool {
	std::string point_id, line_id;
public:
	void processEvent(SDL_Event graphics_event);
};

class CreatePerpendicularBisectorTool : public Tool {
	std::string point_id;
public:
	void processEvent(SDL_Event graphics_event);
};

class CreateMiddlePointTool : public Tool {
	std::string point_id;
public:
	void processEvent(SDL_Event graphics_event);
};

class CreateAngleBisectorTool : public Tool {
	std::string point1_id, point2_id;
public:
	void processEvent(SDL_Event graphics_event);
};

class CreatePolarLineTool : public Tool {
	std::string circle_id, point_id;
public:
	void processEvent(SDL_Event graphics_event);
};

class CreateTangentsTool : public Tool {
	std::string circle_id, point_id;
public:
	void processEvent(SDL_Event graphics_event);
};

class CreateCircleByPointsTool : public Tool {
	std::string point1_id, point2_id;
public:
	void processEvent(SDL_Event graphics_event);
};

class ReflectObjectTool : public Tool {
	std::string object_id;
	cap_t object_cap;
public:
	void processEvent(SDL_Event graphics_event);
};
#endif
