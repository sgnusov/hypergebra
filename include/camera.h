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

	const std::function<ScreenPoint(const Point&)> view2clip;
	const std::function<std::optional<Point>(const ScreenPoint&)> clip2view;

	const std::string shader_lib;

public:
	Camera(const std::function<ScreenPoint(const Point&)>& view2clip,
		const std::function<std::optional<Point>(const ScreenPoint&)>& clip2view,
		const std::string& shader_lib);

	const std::string& getShaderLib();

	ScreenPoint worldToClip(const Point& p);
	std::optional<Point> clipToWorld(const ScreenPoint& p);

	void initShaderProgram(std::unique_ptr<ShaderProgram>& program_ptr);

	static Camera BeltramiKlein();
	static Camera Poincare();
};

#endif
