#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include <string>
#include <unordered_map>

#include <GL/glew.h>

class ShaderProgram {
	GLuint program_id;
	static std::unordered_map<std::string, std::string> libraries;

	void attachShader(GLuint program, GLenum type, const char* code);
	std::string preprocessShader(const std::string& code);

public:
	ShaderProgram(const std::string& vert, const std::string& frag);

	static void addLibrary(const std::string& name, const std::string& code);

	void activate();

	void setf(const std::string& name, float a);
	void setf(const std::string& name, float a, float b, float c);
	void setf(const std::string& name, float a, float b, float c, float d);
	void setMatrix3f(const std::string& name, const long double* arr);
};

#endif
