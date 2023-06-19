#include "object.h"
#include "graphics.h"

Object::Object(const cap_t capability) : capability(capability), render_priority(0), is_selected(false) {}

Object::Object(const cap_t capability, int render_priority)
	: capability(capability), render_priority(render_priority), is_selected(false) {}

int Object::getRenderPriority() const {
	return render_priority;
}

cap_t Object::getCapability() const {
	return capability;
}

void Object::assignProperties(const Properties& properties) {
	if(properties.color.has_value())
		color = properties.color.value();
	if(properties.is_selected.has_value())
		is_selected = properties.is_selected.value();
}

ld GeometryObject::squareScreenDistance(const Point& p) {
	return (active_camera.worldToClip(p) - active_camera.worldToClip(projectPoint(p))).sq();
}

GeometryObject::GeometryObject(const cap_t capability) : Object(capability | GEOMETRY_OBJECT_CAP) {}
GeometryObject::GeometryObject(const cap_t capability, int render_priority) : Object(capability | GEOMETRY_OBJECT_CAP, render_priority) {}
