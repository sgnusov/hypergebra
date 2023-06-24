#include <bits/stdc++.h>
#include <stdio.h>
#include <chrono>

#include <SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
//#include<GL/gl.h>

#include "graphics.h"
#include "shaders.h"
#include "instructions.h"
#include "object_list.h"
#include "tools.h"

int window_width = 1280, window_height = 720;

SDL_Window* window;
SDL_GLContext gl_context;

Camera active_camera = Camera::PoincareHalfPlane();
std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> shader_cache;

std::shared_ptr<Tool> active_tool = std::make_shared<MovePointTool>();

ld getX(int x) {
	ld ratio = (ld)window_width / window_height;
	ld ret = (ld)x / window_width;
	ret = ret * 2 - 1;
	if(ratio > 1)
		ret *= ratio;
	return ret;
}

ld getY(int y) {
	ld ratio = (ld)window_width / window_height;
	ld ret = (ld)y / window_height;
	ret = 1 - ret * 2;
	if(ratio < 1)
		ret /= ratio;
	return ret;
}

void logSdlError(const char* err) {
	std::cerr << err << " Error: " << SDL_GetError();
}

static void getInfo(void) {
	SDL_version compiled;
	SDL_version linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);

	printf("SDL2 comiled version: %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
	printf("SDL2 linked version: %d.%d.%d\n", linked.major, linked.minor, linked.patch);

	printf("OpenGL version: %s\n", glGetString(GL_VERSION));
	printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("Vendor: %s\n", glGetString(GL_VENDOR)); printf("Renderer: %s\n", glGetString(GL_RENDERER));
}

static void openglCallbackFunction(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* user_param
) {
	std::cerr << message << '\n';
	if(severity == GL_DEBUG_SEVERITY_HIGH) {
		std::cerr << "Hight severity OpenGL error. Aborting...\n";
		std::exit(0);
	}
}

void setResolution(int width, int height) {
	window_width = width;
	window_height = height;
	
	float ratio = (float)width / height;
	float x = 1, y = 1;

	if(ratio > 1) {
		x = ratio;
	} else {
		y = 1 / ratio;
	}

	float points[] = {
		-1, -1, 0, 1, -x, -y,
		 1, -1, 0, 1,  x, -y,
		-1,  1, 0, 1, -x,  y,
		 1,  1, 0, 1,  x,  y
	};
	GLuint buffer;
	glCreateBuffers(1, &buffer);
	glNamedBufferStorage(buffer, sizeof(points), points, 0);

	GLuint array;
	glCreateVertexArrays(1, &array);
	int position_attrib = 0;
	int uv_attrib = 4;
	glEnableVertexArrayAttrib(array, position_attrib);
	glEnableVertexArrayAttrib(array, uv_attrib);
	glVertexArrayAttribFormat(array, position_attrib, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(array, uv_attrib, 2, GL_FLOAT, GL_FALSE, 16);
	glVertexArrayAttribBinding(array, position_attrib, 0);
	glVertexArrayAttribBinding(array, uv_attrib, 0);

	glVertexArrayVertexBuffer(array, 0, buffer, 0, sizeof(GLfloat) * 6);
	glBindVertexArray(array);

}

int initGraphics() {

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		logSdlError("SDL initialization failed.");
		return -1;
	}
	
	#define setAttr(attr, value) \
        if (SDL_GL_SetAttribute(attr, value) < 0) { \
            std::cerr << "SDL failed to set " << #attr << " to " << #value << " SDL Error: " << SDL_GetError(); \
        }

	setAttr(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	setAttr(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	setAttr(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	setAttr(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	//setAttr(SDL_GL_DOUBLEBUFFER, 1);
	//setAttr(SDL_GL_DEPTH_SIZE, 24);

	SDL_GL_SetSwapInterval(0);

	window = SDL_CreateWindow("Hypergebra", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if(!window) {
		logSdlError("SDL window creation failed.");
		return -1;
	}

	gl_context = SDL_GL_CreateContext(window);

	if(gl_context == NULL) {
		logSdlError("OpenGL context creation failed.");
	}

	if(SDL_GL_MakeCurrent(window, gl_context)) {
		logSdlError("Activating context failed.");
	}


	GLenum err = glewInit();
	if(err != GLEW_OK) {
		std::cerr << "Glew initialization failed. Error: " << glewGetString(err);
		return -1;
	}

	getInfo();

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openglCallbackFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	SDL_GetWindowSize(window, &window_width, &window_height);
	glViewport(0, 0, window_width, window_height);
	glLineWidth((GLfloat)5); // TODO: Convert it to a shader code

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	setResolution(1280, 720);

	ShaderProgram::addLibrary("camera.lib", active_camera.getShaderLib());

	//instructions.push_back(std::make_unique<CreatePoint>(0, Point(0, 0)));

	//instructions.push_back(std::make_unique<CreatePoint>(0, Point(0.3, -1)));


	instructions.push_back(std::make_unique<CreatePoint>(0, Point(1, 1)));
	instructions.push_back(std::make_unique<CreatePoint>(1, Point(-1, 1)));
	instructions.push_back(std::make_unique<CreatePoint>(2, Point(3, -1)));
	instructions.push_back(std::make_unique<CreateLine>(3, "0", "1"));
	instructions.push_back(std::make_unique<CreateLine>(4, "2", "1"));
	instructions.push_back(std::make_unique<CreateLine>(5, "0", "2"));
	instructions.push_back(std::make_unique<CreatePointOnLine>(6, Point(2, 1), "3"));
	instructions.push_back(std::make_unique<CreatePointOnLine>(7, Point(2, 1), "4"));

	return 0;
}

int mainLoop() {
	SDL_Event event;

	int frames = 0;
	auto start_time = std::chrono::steady_clock::now();
	float fps = 0.0f;
	int last_ticks = SDL_GetTicks();
	while(true) {
		/*
		if(SDL_GetTicks() - last_ticks < 1000 / 100) {
			continue;
		}
		last_ticks = SDL_GetTicks();
		*/
		++frames;
		//std::cerr << "FPS: " << frames / ((std::chrono::duration<double>)(std::chrono::steady_clock::now() - start_time)).count() << '\n';
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		processInstructions();
		objects.render();

		SDL_GL_SwapWindow(window);
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				SDL_GL_DeleteContext(gl_context);
				SDL_DestroyWindow(window);
				SDL_Quit();
				return 0;
			}
			if(event.type == SDL_KEYDOWN) {
				SDL_KeyboardEvent key = event.key;
				switch (key.keysym.sym) {
					case SDLK_m : {
							std::cerr << "Switching to move\n";
							active_tool = std::make_shared<MovePointTool>();
							break;
						}
					case SDLK_p : {
							std::cerr << "Switching to create point\n";
							active_tool = std::make_shared<CreatePointTool>();
							break;
						}
					case SDLK_l : {
							std::cerr << "Switching to create line\n";
							active_tool = std::make_shared<CreateLineTool>();
							break;
						}
					case SDLK_i : {
							std::cerr << "Switching to line intersection\n";
							active_tool = std::make_shared<IntersectLinesTool>();
							break;
						}
					case SDLK_h : {
							std::cerr << "Switching to hiding objects\n";
							active_tool = std::make_shared<HideObjectTool>();
							break;
						}
				}
				deselectObjects();
			}
			//std::cerr << "Got event\n";
			active_tool->processEvent(event);
		}
	}
}
