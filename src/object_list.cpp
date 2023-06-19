#include <vector>
#include <algorithm>

#include "object_list.h"

ObjectList objects;

void ObjectList::add(const std::string& id, int origin_id, const std::shared_ptr<Object>& obj_ptr) {
	object_list[id] = obj_ptr;
	origin[id] = origin_id;
}

ObjectList::const_iterator ObjectList::begin() const {
	return object_list.begin();
}

ObjectList::const_iterator ObjectList::end() const {
	return object_list.end();
}

int ObjectList::getOrigin(const std::string& id) {
	return origin[id];
}

void ObjectList::render() {
	std::vector<int> priorities;
	for(auto [id, object] : object_list) {
		if(object != nullptr)
			priorities.push_back(object->getRenderPriority());
	}
	std::sort(priorities.begin(), priorities.end());
	priorities.resize(std::unique(priorities.begin(), priorities.end()) - priorities.begin());
	for(int priority : priorities) {
		for(auto [id, object] : object_list) {
			if(object != nullptr && object->getRenderPriority() == priority) {
				object->assignProperties(properties[id]);
				object->render();
			}
		}
	}
}

void ObjectList::clear() {
	object_list.clear();
}

const std::shared_ptr<Object>& ObjectList::operator [] (const std::string& id) {
	return object_list[id];
}
