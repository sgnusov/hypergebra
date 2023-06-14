#ifndef OBJECT_LIST_H_
#define OBJECT_LIST_H_

#include <map>
#include <string>

#include "object.h"

class ObjectList {
	std::map<std::string, std::shared_ptr<Object>> object_list;
	std::map<std::string, int> origin;

public:
	using const_iterator = std::map<std::string, std::shared_ptr<Object>>::const_iterator;
	
	const_iterator begin() const;
	const_iterator end() const;

	void add(const std::string& id, int origin_id, const std::shared_ptr<Object>& obj_ptr);
	int getOrigin(const std::string& id);

	void clear();
	void render();
	
	const std::shared_ptr<Object>& operator [] (const std::string& id) const;
};

extern ObjectList objects;

#endif
