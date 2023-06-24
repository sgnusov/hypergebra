#ifndef OBJECT_H_
#define OBJECT_H_

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <optional>

using ld = long double;
using cap_t = uint32_t;

const cap_t GEOMETRY_OBJECT_CAP = 0x1;

struct Properties {
	std::optional<std::vector<unsigned char>> color;
	std::optional<bool> is_selected;
	std::optional<bool> is_hidden;
};

class Object {
	int render_priority;

protected:
	cap_t capability;

public:
	std::vector<unsigned char> color;
	bool is_selected;
	bool is_hidden;

public:
	Object(cap_t capability);
	Object(cap_t capability, int render_priority);
	virtual ~Object() = default;

	virtual void render() = 0;
	int getRenderPriority() const;
	cap_t getCapability() const;
	void assignProperties(const Properties& properties);
};

class Point;

class GeometryObject : public Object {
public:
	GeometryObject(cap_t capability);
	GeometryObject(cap_t capability, int render_priority);
	virtual Point projectPoint(const Point& p) = 0;
	virtual ld squareScreenDistance(const Point& p);
};

#endif
