#include "tools.h"
#include "graphics.h"
#include "object_list.h"
#include "instructions.h"

const ld ACTIVATION_DISTANCE = 1e-3;

void MovePointTool::processEvent(SDL_Event event_) {
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
						if(object_ptr != nullptr && (object_ptr->getCapability() & MOVABLE_POINT_CAP)) {
							ld dist = std::static_pointer_cast<GeometryObject>(object_ptr)->squareScreenDistance(p);
							if(dist < min) {
								min = dist;
								origin_id = objects.getOrigin(id);
								is_point_on_line = (object_ptr->getCapability() & POINT_ON_LINE_CAP);
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
				if(is_point_on_line) {
					instruction = std::make_unique<CreatePointOnLine>(instruction->getId(), p_.value(),
						static_cast<CreatePointOnLine*>(instruction.get())->getLineId());

				} else {
					instruction = std::make_unique<CreatePoint>(instruction->getId(), p_.value());
				}
				break;
			}
	}
}

void CreatePointTool::processEvent(SDL_Event event_) {
	switch(event_.type) {
		case SDL_MOUSEBUTTONDOWN : 
			{
				SDL_MouseButtonEvent event = event_.button;
				if(event.button != SDL_BUTTON_LEFT)
					break;
				ld x = getX(event.x);
				ld y = getY(event.y);
				ScreenPoint click(x, y);
				std::optional<Point> p_ = active_camera.clipToWorld(click);
				if(p_ == std::nullopt) {
					break;
				}
				Point p = p_.value();
				std::string min_id = "";
				ld min = ACTIVATION_DISTANCE;
				for(auto [id, object_ptr] : objects) {
					if(object_ptr != nullptr && (object_ptr->getCapability() & LINE_CAP)) {
						ld dist = std::static_pointer_cast<Line>(object_ptr)->squareScreenDistance(p);
						if(dist < min) {
							min = dist;
							min_id = id;
						}
					}
				}
				if(min_id != "") {
					instructions.push_back(std::make_unique<CreatePointOnLine>(instructions.size() , p, min_id));
				} else {
					instructions.push_back(std::make_unique<CreatePoint>(instructions.size(), p));
				}
				break;
			}
	}
}


void CreateLineTool::processEvent(SDL_Event event_) {
	switch(event_.type) {
		case SDL_MOUSEBUTTONDOWN : 
			{
				SDL_MouseButtonEvent event = event_.button;
				if(event.button != SDL_BUTTON_LEFT)
					break;
				ld x = getX(event.x);
				ld y = getY(event.y);
				ScreenPoint click(x, y);
				std::optional<Point> p_ = active_camera.clipToWorld(click);
				if(p_ == std::nullopt) {
					break;
				}
				Point p = p_.value();
				std::string min_id = "";
				ld min = ACTIVATION_DISTANCE;
				for(auto [id, object_ptr] : objects) {
					if(object_ptr != nullptr && (object_ptr->getCapability() & MOVABLE_POINT_CAP)) {
						ld dist = std::static_pointer_cast<GeometryObject>(object_ptr)->squareScreenDistance(p);
						if(dist < min) {
							min = dist;
							min_id = id;
						}
					}
				}
				if(min_id != "") {
					if(point_id == "") {
						point_id = min_id;
						break;
					}
					if(point_id == min_id) {
						break;
					}
					instructions.push_back(std::make_unique<CreateLine>(instructions.size(), point_id, min_id));
				} 
				break;
			}
	}
}

void IntersectLinesTool::processEvent(SDL_Event event_) {
	switch(event_.type) {
		case SDL_MOUSEBUTTONDOWN : 
			{
				SDL_MouseButtonEvent event = event_.button;
				if(event.button != SDL_BUTTON_LEFT)
					break;
				ld x = getX(event.x);
				ld y = getY(event.y);
				ScreenPoint click(x, y);
				std::optional<Point> p_ = active_camera.clipToWorld(click);
				if(p_ == std::nullopt) {
					break;
				}
				Point p = p_.value();
				std::string min_id = "";
				ld min = ACTIVATION_DISTANCE;
				for(auto [id, object_ptr] : objects) {
					if(object_ptr != nullptr && (object_ptr->getCapability() & LINE_CAP)) {
						ld dist = std::static_pointer_cast<GeometryObject>(object_ptr)->squareScreenDistance(p);
						if(dist < min) {
							min = dist;
							min_id = id;
						}
					}
				}
				if(min_id != "") {
					if(line_id == "") {
						line_id = min_id;
						break;
					}
					if(line_id == min_id) {
						break;
					}
					instructions.push_back(std::make_unique<IntersectLines>(instructions.size(), line_id, min_id));
				} 
				break;
			}
	}
}
