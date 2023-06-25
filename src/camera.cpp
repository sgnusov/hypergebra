#include <cmath>

#include "camera.h"
#include "shaders.h"

Camera::Camera(const std::function<ScreenPoint(const Point&)>& view2clip,
	const std::function<std::optional<Point>(const ScreenPoint&)>& clip2view,
	const std::string& shader_lib) : view2clip(view2clip), clip2view(clip2view), shader_lib(shader_lib) {
	view2world = Transformation::identity();
	world2view = Transformation::identity();
}

const std::string& Camera::getShaderLib() {
	return shader_lib;
}

ScreenPoint Camera::worldToClip(const Point& p) {
	return view2clip(world2view * p);
}

std::optional<Point> Camera::clipToWorld(const ScreenPoint& p) {
	std::optional<Point> res = clip2view(p);
	if(res == std::nullopt)
		return std::nullopt;
	return view2world * res.value();
}

ScreenPoint Camera::viewToClip(const Point& p) {
	return view2clip(p);
}

std::optional<Point> Camera::clipToView(const ScreenPoint& p) {
	std::optional<Point> res = clip2view(p);
	if(res == std::nullopt)
		return std::nullopt;
	return res.value();
}

void Camera::initShaderProgram(std::unique_ptr<ShaderProgram>& program_ptr) {
	program_ptr->setMatrix3f("view2world", view2world);
	program_ptr->setMatrix3f("world2view", world2view);
}

void Camera::move(Transformation trans) {
	world2view = trans * world2view;
	world2view.correct();
	view2world = ~world2view;
}

Camera Camera::BeltramiKlein() {
	return Camera(
		[](const Point& p){ return ScreenPoint(p[0] / p[2], p[1] / p[2]); },
		[](const ScreenPoint& p) -> std::optional<Point> {
			ld mod = -(p[0] * p[0] + p[1] * p[1] - 1);
			if(mod <= 0) {
				return std::nullopt;
			}
			mod = 1 / std::sqrt(mod);
			return Point(p[0] * mod, p[1] * mod, 1 * mod);
		},
		beltrami_klein_lib
	);
}

Camera Camera::PoincareDisk() {
	return Camera(
		[](const Point& p){ return ScreenPoint(p[0] / (p[2] + 1), p[1] / (p[2] + 1)); },
		[](const ScreenPoint& p) -> std::optional<Point> {
			ld mod = (1 - p[0] * p[0] - p[1] * p[1]);
			if(mod <= 0) {
				return std::nullopt;
			}
			mod = 2 / mod;
			return Point(p[0] * mod, p[1] * mod, mod - 1);
		},
		poincare_disk_lib
	);
}

Camera Camera::PoincareHalfPlane() {
	return Camera(
		[](const Point& p){
			Point p1 = p;
			p1 = p1 / (p1[2] + 1);
			p1[1] += 1;
			p1[2] = 0;
			p1 = p1 / p1.sq() * 2;
			return ScreenPoint(p1[0], p1[1] - 2);
		},
		[](const ScreenPoint& p) -> std::optional<Point> {
			if(p[1] <= -1) {
				return std::nullopt;
			}
			Point res(p[0], p[1] + 2, 0);
			res = res / res.sq() * 2;
			res[1] -= 1;
			ld mod = (1 - res[0] * res[0] - res[1] * res[1]);
			if(mod <= 0) {
				return std::nullopt;
			}
			mod = 2 / mod;
			return Point(res[0] * mod, res[1] * mod, mod - 1);
		},
		poincare_half_plane_lib
	);
}
