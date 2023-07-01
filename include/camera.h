#ifndef CAMERA_H_
#define CAMERA_H_

#include <functional>
#include <string>
#include <optional>
#include <memory>

#include "geometry.h"
#include "shader_program.h"
#include "screen_point.h"

class Camera {
	Transformation world2view;
	Transformation view2world;

	std::function<ScreenPoint(const Point&)> view2clip;
	std::function<std::optional<Point>(const ScreenPoint&)> clip2view;

	std::string shader_lib;

public:
	Camera(const std::function<ScreenPoint(const Point&)>& view2clip,
		const std::function<std::optional<Point>(const ScreenPoint&)>& clip2view,
		const std::string& shader_lib);

	const std::string& getShaderLib();

	ScreenPoint worldToClip(const Point& p);
	std::optional<Point> clipToWorld(const ScreenPoint& p);

	ScreenPoint viewToClip(const Point& p);
	std::optional<Point> clipToView(const ScreenPoint& p);

	void initShaderProgram(std::unique_ptr<ShaderProgram>& program_ptr);

	void move(Transformation trans);

	static Camera BeltramiKlein();
	static Camera PoincareDisk();
	static Camera PoincareHalfPlane();
};

#endif
