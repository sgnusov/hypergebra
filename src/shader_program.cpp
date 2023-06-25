#include "shader_program.h"

#include <GL/glew.h>
#include <iostream>

std::unordered_map<std::string, std::string> ShaderProgram::libraries;

void ShaderProgram::attachShader(GLuint program, GLenum type, const char* code) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);
	glAttachShader(program, shader);
	glDeleteShader(shader);
}

std::string ShaderProgram::preprocessShader(const std::string& code) {
	static const std::string prefix = "#include";
	std::string res;
	std::string string;
	std::string library_name;
	bool good_prefix = true;
	bool parsing_library_name = false;
	for(char c : code) {
		string += c;
		if(string.size() <= prefix.size()) {
			if(string.back() != prefix[string.size() - 1]) {
				good_prefix = false;
			}
		} else if(good_prefix) {
			if(c == '\"') {
				if(parsing_library_name) {
					res += libraries[library_name] + '\n';
					string = "";
				}
				parsing_library_name ^= 1;
			} else if(parsing_library_name) {
				library_name += c;
			}
				
		}
		if(c == '\n') {
			good_prefix = true;
			res += string;
			string = "";
			continue;
		}
	}
	return res;
}

ShaderProgram::ShaderProgram(const std::string& vert, const std::string& frag) {
	program_id = glCreateProgram();

	attachShader(program_id, GL_VERTEX_SHADER, preprocessShader(vert).c_str());
	attachShader(program_id, GL_FRAGMENT_SHADER, preprocessShader(frag).c_str());

	glLinkProgram(program_id);

	GLint result;
	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	if (result != GL_TRUE) {
		char msg[10240];
		glGetProgramInfoLog(program_id, 10240, NULL, msg);
		std::cerr << "Program linking failed:\n" << msg;
		std::exit(-1);
	}
}

void ShaderProgram::addLibrary(const std::string& name, const std::string& code) {
	ShaderProgram::libraries[name] = code;
}

void ShaderProgram::activate() {
	glUseProgram(program_id);
}

void ShaderProgram::setf(const std::string& name, float a) {
	glUniform1f(glGetUniformLocation(program_id, name.c_str()), a);
}

void ShaderProgram::setf(const std::string& name, float a, float b, float c) {
	glUniform3f(glGetUniformLocation(program_id, name.c_str()), a, b, c);
}

void ShaderProgram::setf(const std::string& name, float a, float b, float c, float d) {
	glUniform4f(glGetUniformLocation(program_id, name.c_str()), a, b, c, d);
}

void ShaderProgram::seti(const std::string& name, int a) {
	glUniform1i(glGetUniformLocation(program_id, name.c_str()), a);
}
void ShaderProgram::setMatrix3f(const std::string& name, const long double* arr) {
	float arr_[9];
	for(int i = 0; i < 9; ++i)
		arr_[i] = arr[i];
	glUniformMatrix3fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_TRUE, arr_);
}
