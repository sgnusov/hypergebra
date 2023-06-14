#include "object.h"
#include "graphics.h"

Object::Object(const std::string& type) : type(type), base_type("Object"), render_priority(0) {}

Object::Object(const std::string& type, const std::string& base_type) : type(type), base_type(base_type), render_priority(0) {}
Object::Object(const std::string& type, const std::string& base_type, int render_priority)
	: type(type), base_type(base_type), render_priority(render_priority) {}

int Object::getRenderPriority() {
	return render_priority;
}

ld GeometryObject::squareScreenDistance(const Point& p) {
	return (active_camera.worldToClip(p) - active_camera.worldToClip(projectPoint(p))).sq();
}

GeometryObject::GeometryObject(const std::string& type) : Object(type, "GeometryObject") {}
GeometryObject::GeometryObject(const std::string& type, int render_priority) : Object(type, "GeometryObject", render_priority) {}
