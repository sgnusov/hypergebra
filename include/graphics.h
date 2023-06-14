#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <memory>

#include "camera.h"
#include "shader_program.h"

extern const ld INF;

extern int window_width, window_height;

extern Camera active_camera;
extern std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> shader_cache;

int initGraphics();
int mainLoop();

ld getX(int x);
ld getY(int y);

#endif
