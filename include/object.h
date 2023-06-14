#ifndef OBJECT_H_
#define OBJECT_H_

#include <unordered_map>
#include <memory>
#include <string>

using ld = long double;

class Object {
	int render_priority;

public:
	std::string type;
	std::string base_type;
	Object(const std::string& type);
	Object(const std::string& type, const std::string& base_type);
	Object(const std::string& type, const std::string& base_type, int render_priority);
	virtual ~Object() = default;

	virtual void render() = 0;
	int getRenderPriority();
};

class Point;

class GeometryObject : public Object {
public:
	GeometryObject(const std::string& type);
	GeometryObject(const std::string& type, int render_priority);
	virtual Point projectPoint(const Point& p) = 0;
	virtual ld squareScreenDistance(const Point& p);
};

#endif
