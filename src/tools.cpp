#include <iostream>

#include "tools.h"
#include "graphics.h"
#include "object_list.h"
#include "instructions.h"

const ld ACTIVATION_DISTANCE = 1e-3;

void selectObject(const std::string& id) {
	objects.properties[id].is_selected = true;
}

void deselectObjects() {
	for(auto& [id, property] : objects.properties)
		property.is_selected = false;
}

std::string findObject(const Point& p, cap_t cap, cap_t acap) {
	std::string min_id = "";
	ld min = ACTIVATION_DISTANCE;
	for(auto [id, object_ptr] : objects) {
		if(object_ptr != nullptr && (object_ptr->getCapability() & cap) && !(object_ptr->getCapability() & acap)) {
			ld dist = std::static_pointer_cast<GeometryObject>(object_ptr)->squareScreenDistance(p);
			if(dist < min) {
				min = dist;
				min_id = id;
			}
		}
	}
	return min_id;
}

std::pair<std::string, cap_t> findObjectCap(const Point& p, cap_t cap, cap_t acap) {
	std::string min_id = "";
	ld min = ACTIVATION_DISTANCE;
	cap_t rcap;
	for(auto [id, object_ptr] : objects) {
		if(object_ptr != nullptr && (object_ptr->getCapability() & cap) && !(object_ptr->getCapability() & acap)) {
			ld dist = std::static_pointer_cast<GeometryObject>(object_ptr)->squareScreenDistance(p);
			if(dist < min) {
				min = dist;
				min_id = id;
				rcap = object_ptr->getCapability();
			}
		}
	}
	return {min_id, rcap};
}
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
					std::string min_id;
					std::tie(min_id, capability) = findObjectCap(p, MOVABLE_POINT_CAP, 0);
					if(min_id == "")
						break;
					selectObject(min_id);
					origin_id = objects.getOrigin(min_id);
					current_instruction_id = origin_id;
				} else {
					deselectObjects();
					current_instruction_id = -1;
				}
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
				if(capability & POINT_ON_LINE_CAP) {
					instruction = std::make_unique<CreatePointOnLine>(instruction->getId(), p_.value(),
						static_cast<CreatePointOnLine*>(instruction.get())->getLineId());

				} else if(capability & POINT_ON_CIRCLE_CAP) {
					instruction = std::make_unique<CreatePointOnCircle>(instruction->getId(), p_.value(),
						static_cast<CreatePointOnCircle*>(instruction.get())->getCircleId());
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
				ld min = ACTIVATION_DISTANCE;
				cap_t capability;
				std::string min_id;
				std::tie(min_id, capability) = findObjectCap(p, GEOMETRY_OBJECT_CAP, POINT_CAP);
				if(min_id != "") {
					if(capability & LINE_CAP) {
						instructions.push_back(std::make_unique<CreatePointOnLine>(instructions.size(), p, min_id));
					} else if(capability & CIRCLE_CAP) {
						instructions.push_back(std::make_unique<CreatePointOnCircle>(instructions.size(), p, min_id));
					}
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
				std::string min_id = findObject(p, POINT_CAP, 0);
				if(min_id == "")
					break;
				if(point_id == "") {
					point_id = min_id;
					selectObject(min_id);
					break;
				}
				if(point_id == min_id) {
					break;
				}
				instructions.push_back(std::make_unique<CreateLine>(instructions.size(), point_id, min_id));
				point_id = "";
				deselectObjects();
				break;
			}
	}
}

void IntersectObjectsTool::processEvent(SDL_Event event_) {
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
				std::string min_id;
				cap_t cap;
				std::tie(min_id, cap) = findObjectCap(p, ~((cap_t)1), POINT_CAP);
				if(min_id != "") {
					if(object_id == "") {
						object_id = min_id;
						object_cap = cap;
						selectObject(min_id);
						break;
					}
					if(object_id == min_id) {
						break;
					}
					if(cap & LINE_CAP) {
						std::swap(cap, object_cap);
						std::swap(min_id, object_id);
					}
					if(object_cap & LINE_CAP) {
						if(cap & LINE_CAP) {
							instructions.push_back(std::make_unique<IntersectLines>(instructions.size(), object_id, min_id));
						}
						if(cap & CIRCLE_CAP) {
							instructions.push_back(std::make_unique<IntersectCircleLine>(instructions.size(), min_id, object_id));
						}
					} else if (object_cap & CIRCLE_CAP) {
						if(cap & CIRCLE_CAP) {
							instructions.push_back(std::make_unique<IntersectCircles>(instructions.size(), object_id, min_id));
						}
					}
					object_id = "";
					deselectObjects();
				} 
				break;
			}
	}
}


void HideObjectTool::processEvent(SDL_Event event_) {
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
				std::string min_id = findObject(p, POINT_CAP, 0);
				if(min_id != "") {
					std::optional<bool>& ref = objects.properties[min_id].is_hidden;
					if(ref.has_value())
						ref = ref.value() ^ 1;
					else
						ref = true;
					break;
				}
				min_id = findObject(p, GEOMETRY_OBJECT_CAP, 0);
				if(min_id != "") {
					std::optional<bool>& ref = objects.properties[min_id].is_hidden;
					if(ref.has_value())
						ref = ref.value() ^ 1;
					else
						ref = true;
					break;
				}
				break;
			}
	}
}

HideObjectTool::HideObjectTool() {
	show_hidden = true;
}

HideObjectTool::~HideObjectTool() {
	show_hidden = false;
}


void DeleteObjectTool::processEvent(SDL_Event event_) {
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
				std::string min_id = findObject(p, POINT_CAP, 0);
				if(min_id != "") {
					instructions.push_back(std::make_unique<DeleteObject>(instructions.size(), objects.getOrigin(min_id)));
					break;
				}
				min_id = findObject(p, GEOMETRY_OBJECT_CAP, 0);
				if(min_id != "") {
					instructions.push_back(std::make_unique<DeleteObject>(instructions.size(), objects.getOrigin(min_id)));
				}
				break;
			}
	}
}

void MoveCameraTool::processEvent(SDL_Event event_) {
	switch(event_.type) {
		case SDL_MOUSEBUTTONDOWN : 
			{
				SDL_MouseButtonEvent event = event_.button;
				if(event.button != SDL_BUTTON_LEFT)
					break;
				ld x = getX(event.x);
				ld y = getY(event.y);
				ScreenPoint click(x, y);
				std::optional<Point> p_ = active_camera.clipToView(click);
				if(p_ == std::nullopt) {
					break;
				}
				Point p = p_.value();
				last_mouse_position = p;
				//active_camera.move(Transformation::rotateToPoint(p));
				//active_camera.move(Transformation::moveToPoint(p));
				break;
			}
		case SDL_MOUSEBUTTONUP : 
			{
				last_mouse_position = Point(0, 0, 0);
				break;
			}
		case SDL_MOUSEMOTION :
			{
				SDL_MouseButtonEvent event = event_.button;
				if(event.button != SDL_BUTTON_LEFT)
					break;
				ld x = getX(event.x);
				ld y = getY(event.y);
				ScreenPoint click(x, y);
				std::optional<Point> p_ = active_camera.clipToView(click);
				if(p_ == std::nullopt) {
					break;
				}
				Point p = p_.value();
				//active_camera.move(Transformation::moveToPoint(p));
				active_camera.move(Transformation::movePointToPoint(last_mouse_position, p));
				last_mouse_position = p;
				break;
			}
	}
}


void CreateCircleTool::processEvent(SDL_Event event_) {
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
				std::string min_id = findObject(p, POINT_CAP, 0);
				if(min_id == "")
					break;
				if(center_id == "") {
					center_id = min_id;
					selectObject(min_id);
					break;
				}
				if(center_id == min_id) {
					break;
				}
				instructions.push_back(std::make_unique<CreateCircle>(instructions.size(), center_id, min_id));
				center_id = "";
				deselectObjects();
				break;
			}
	}
}

void CreatePerpendicularLineTool::processEvent(SDL_Event event_) {
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
				std::string min_id;
				cap_t cap;
				bool object_found = false;
				if(point_id == "") {
					std::tie(min_id, cap) = findObjectCap(p, POINT_CAP, 0);
					if(min_id != "") {
						point_id = min_id;
						selectObject(point_id);
						object_found = true;
					}
				}
				if(!object_found && line_id == "") {
					std::tie(min_id, cap) = findObjectCap(p, LINE_CAP, 0);
					if(min_id != "") {
						line_id = min_id;
						selectObject(line_id);
					}
				}
				if(line_id != "" && point_id != "") {
					instructions.push_back(std::make_unique<PerpendicularPointLine>(instructions.size(), point_id, line_id));
					line_id = "";
					point_id = "";
					deselectObjects();
				}
				break;
			}
	}
}

void CreatePerpendicularBisectorTool::processEvent(SDL_Event event_) {
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
				std::string min_id = findObject(p, POINT_CAP, 0);
				if(min_id == "")
					break;
				if(point_id == "") {
					point_id = min_id;
					selectObject(min_id);
					break;
				}
				if(point_id == min_id) {
					break;
				}
				instructions.push_back(std::make_unique<PerpendicularBisector>(instructions.size(), point_id, min_id));
				point_id = "";
				deselectObjects();
				break;
			}
	}
}

void CreateMiddlePointTool::processEvent(SDL_Event event_) {
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
				std::string min_id = findObject(p, POINT_CAP, 0);
				if(min_id == "") {
					min_id = findObject(p, CIRCLE_CAP, 0);
					if(min_id != "") {
						instructions.push_back(std::make_unique<CircleCenter>(instructions.size(), min_id));
						deselectObjects();
					}
				}
				if(point_id == "") {
					point_id = min_id;
					selectObject(min_id);
					break;
				}
				if(point_id == min_id) {
					break;
				}
				instructions.push_back(std::make_unique<MiddlePoint>(instructions.size(), point_id, min_id));
				point_id = "";
				deselectObjects();
				break;
			}
	}
}

void CreateAngleBisectorTool::processEvent(SDL_Event event_) {
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
				std::string min_id = findObject(p, POINT_CAP, 0);
				if(min_id == "")
					break;
				if(min_id == point1_id || min_id == point2_id)
					break;
				if(point1_id == "") {
					point1_id = min_id;
					selectObject(min_id);
					break;
				}
				if(point2_id == "") {
					point2_id = min_id;
					selectObject(min_id);
					break;
				}
				instructions.push_back(std::make_unique<AngleBisector>(instructions.size(), point1_id, point2_id, min_id));
				point1_id = "";
				point2_id = "";
				deselectObjects();
				break;
			}
	}
}

void CreatePolarLineTool::processEvent(SDL_Event event_) {
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
				std::string min_id;
				cap_t cap;
				bool object_found = false;
				if(point_id == "") {
					std::tie(min_id, cap) = findObjectCap(p, POINT_CAP, 0);
					if(min_id != "") {
						point_id = min_id;
						selectObject(point_id);
						object_found = true;
					}
				}
				if(!object_found && circle_id == "") {
					std::tie(min_id, cap) = findObjectCap(p, CIRCLE_CAP, 0);
					if(min_id != "") {
						circle_id = min_id;
						selectObject(circle_id);
					}
				}
				if(circle_id != "" && point_id != "") {
					instructions.push_back(std::make_unique<PolarLine>(instructions.size(), circle_id, point_id));
					circle_id = "";
					point_id = "";
					deselectObjects();
				}
				break;
			}
	}
}

void CreateTangentsTool::processEvent(SDL_Event event_) {
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
				std::string min_id;
				cap_t cap;
				bool object_found = false;
				if(point_id == "") {
					std::tie(min_id, cap) = findObjectCap(p, POINT_CAP, 0);
					if(min_id != "") {
						point_id = min_id;
						selectObject(point_id);
						object_found = true;
					}
				}
				if(!object_found && circle_id == "") {
					std::tie(min_id, cap) = findObjectCap(p, CIRCLE_CAP, 0);
					if(min_id != "") {
						circle_id = min_id;
						selectObject(circle_id);
					}
				}
				if(circle_id != "" && point_id != "") {
					instructions.push_back(std::make_unique<Tangents>(instructions.size(), circle_id, point_id));
					circle_id = "";
					point_id = "";
					deselectObjects();
				}
				break;
			}
	}
}

void CreateCircleByPointsTool::processEvent(SDL_Event event_) {
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
				std::string min_id = findObject(p, POINT_CAP, 0);
				if(min_id == "")
					break;
				if(min_id == point1_id || min_id == point2_id)
					break;
				if(point1_id == "") {
					point1_id = min_id;
					selectObject(min_id);
					break;
				}
				if(point2_id == "") {
					point2_id = min_id;
					selectObject(min_id);
					break;
				}
				instructions.push_back(std::make_unique<CircleByPoints>(instructions.size(), point1_id, point2_id, min_id));
				point1_id = "";
				point2_id = "";
				deselectObjects();
				break;
			}
	}
}

void ReflectObjectTool::processEvent(SDL_Event event_) {
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
				std::string min_id;
				cap_t cap;
				if(object_id == "") {
					std::tie(min_id, cap) = findObjectCap(p, POINT_CAP, 0);
					if(min_id == "") {
						std::tie(min_id, cap) = findObjectCap(p, GEOMETRY_OBJECT_CAP, 0);
					}
					if(min_id == "") {
						break;
					}
					object_id = min_id;
					object_cap = cap;
					selectObject(min_id);
					break;
				}
				min_id = findObject(p, LINE_CAP, 0);
				if(min_id == "" || min_id == object_id) {
					break;
				}
				instructions.push_back(std::make_unique<ReflectObject>(instructions.size(), object_id, min_id));
				object_id = "";
				deselectObjects();
				break;
			}
	}
}
