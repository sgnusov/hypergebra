#include <memory>
#include <iostream>

#include "geometry.h"
#include "graphics.h"
#include "shader_program.h"
#include "shaders.h"

void Point::render() {
	std::unique_ptr<ShaderProgram>& shader_ptr = shader_cache["Point"];
	if(shader_ptr == nullptr) {
		shader_ptr = std::make_unique<ShaderProgram>(point_vert, point_frag);
	}
	shader_ptr->activate();
	active_camera.initShaderProgram(shader_ptr);
	shader_ptr->setf("coords", coords[0], coords[1], coords[2]);
	shader_ptr->setf("radius", 0.015);
	shader_ptr->setf("ratio", (float)window_width / window_height);
	shader_ptr->setf("color", color[0] / 255.0, color[1] / 255.0, color[2] / 255.0);
	if(is_selected) {
		shader_ptr->seti("is_selected", true);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	shader_ptr->seti("is_selected", false);
	if(is_hidden) {
		shader_ptr->seti("is_hidden", true);
	} else {
		shader_ptr->seti("is_hidden", false);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}


void Line::render() {
	std::unique_ptr<ShaderProgram>& shader_ptr = shader_cache["Line"];
	if(shader_ptr == nullptr) {
		shader_ptr = std::make_unique<ShaderProgram>(line_vert, line_frag);
	}
	shader_ptr->activate();
	active_camera.initShaderProgram(shader_ptr);
	shader_ptr->setf("l", coords[0], coords[1], coords[2]);
/*
	Point p = getPoint();
	Point tg = getTangent(p);
	shader_ptr->setf("x0", p[0], p[1], p[2]);
	shader_ptr->setf("v0", tg[0], tg[1], tg[2]);
*/
	shader_ptr->setf("width", 0.006);
	shader_ptr->setf("ratio", (float)window_width / window_height);
	if(is_selected) {
		shader_ptr->seti("is_selected", true);
	} else {
		shader_ptr->seti("is_selected", false);
	}
	if(is_hidden) {
		shader_ptr->seti("is_hidden", true);
	} else {
		shader_ptr->seti("is_hidden", false);
	}
	shader_ptr->setf("color", color[0] / 255.0, color[1] / 255.0, color[2] / 255.0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}
