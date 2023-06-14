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


class MovePoint : public Tool {
	int current_instruction_id = -1;
public:
	void processEvent(SDL_Event graphics);
};

#endif
