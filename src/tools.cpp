#include "tools.h"
#include "graphics.h"
#include "object_list.h"
#include "instructions.h"

const ld ACTIVATION_DISTANCE = 1e-3;

void MovePoint::processEvent(SDL_Event event_) {
	switch(event_.type) {
		case SDL_MOUSEBUTTONUP :
		case SDL_MOUSEBUTTONDOWN : 
			{
				SDL_MouseButtonEvent event = event_.button;
				if(event.button != SDL_BUTTON_LEFT)
					break;
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					ld x = getX(event.x);
					ld y = getY(event.y);
					ScreenPoint click(x, y);
					std::optional<Point> p_ = active_camera.clipToWorld(click);
					if(p_ == std::nullopt) {
						break;
					}
					Point p = p_.value();
					int origin_id = -1;
					ld min = ACTIVATION_DISTANCE;
					for(auto [id, object_ptr] : objects) {
						if(object_ptr->type == "Point") {
							ld dist = std::static_pointer_cast<Point>(object_ptr)->squareScreenDistance(p);
							if(dist < min) {
								min = dist;
								origin_id = objects.getOrigin(id);
							}
						}
					}
					current_instruction_id = origin_id;
				} else {
					current_instruction_id = -1;
				}
				break;
			}
		case SDL_MOUSEMOTION :
			{
				SDL_MouseMotionEvent event = event_.motion;
				if(current_instruction_id == -1)
					break;
				ld x = getX(event.x);
				ld y = getY(event.y);
				ScreenPoint click(x, y);
				std::optional<Point> p_ = active_camera.clipToWorld(click);
				if(p_ == std::nullopt) {
					break;
				}
				std::unique_ptr<Instruction>& instruction = instructions[current_instruction_id];
				instruction = std::make_unique<CreatePoint>(instruction->getId(), p_.value());
				break;
			}
	}
}
