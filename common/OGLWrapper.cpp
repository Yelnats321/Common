#include "OGLWrapper.h"
#include <fstream>
#include <sstream>
#include <iostream>


//extern "C"{
//#include <SOIL.h>
//}

//#include "Material.h"
//#include "Mesh.h"
//#include "ObjFile.h"


namespace gl {
using std::string;

Program::Program() {}

Program::Program(Program && rhs) {
	std::swap(data, rhs.data);
}
Program& Program::operator=(Program&& rhs) {
	std::swap(data, rhs.data);
	return *this;
}

Program::~Program() {
	glDeleteProgram(data);
}

void checkShader(const string & name, GLuint shader) {
	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status) {
		std::cout << name << " compiled" << std::endl;
	}
	else {
		std::cerr << name << " compile error" << std::endl;
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		if (length) {
			GLint infoLength;
			char* infoBuf = new char[length];
			glGetShaderInfoLog(shader, length, &infoLength, infoBuf);
			std::cerr << infoBuf;
			delete[] infoBuf;
		}
	}
}

void Program::gen(string vert, string frag, string geo) {
	std::ifstream file;

	string vertexSource, fragmentSource;
	file.open(vert);
	if (file) {
		file.seekg(0, std::ios::end);
		vertexSource.resize(static_cast<string::size_type>(file.tellg()));
		file.seekg(0, std::ios::beg);
		file.read(&vertexSource[0], vertexSource.size());
		file.close();
	}
	else throw "No vertex shader";

	file.open(frag);
	if (file) {
		file.seekg(0, std::ios::end);
		fragmentSource.resize(static_cast<string::size_type>(file.tellg()));
		file.seekg(0, std::ios::beg);
		file.read(&fragmentSource[0], fragmentSource.size());
		file.close();
	}
	else throw "No fragment shader";

	string geometrySource;
	if (!geo.empty()) {
		file.open(geo);
		if (file) {
			file.seekg(0, std::ios::end);
			geometrySource.resize(static_cast<string::size_type>(file.tellg()));
			file.seekg(0, std::ios::beg);
			file.read(&geometrySource[0], geometrySource.size());
			file.close();
		}
		else throw "No geometry shader";
	}

	// Create and compile the vertex shader
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	const char *c_str1 = vertexSource.c_str();
	glShaderSource(vertShader, 1, &c_str1, nullptr);
	glCompileShader(vertShader);

	// Create and compile the fragment shader
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char *c_str2 = fragmentSource.c_str();
	glShaderSource(fragShader, 1, &c_str2, nullptr);
	glCompileShader(fragShader);

	checkShader(vert, vertShader);
	checkShader(frag, fragShader);
	GLuint geoShader = 0;
	if (!geo.empty()) {
		geoShader = glCreateShader(GL_GEOMETRY_SHADER);
		const char *c_str3 = geometrySource.c_str();
		glShaderSource(geoShader, 1, &c_str3, nullptr);
		glCompileShader(geoShader);
		checkShader(geo, geoShader);
	}
	endl(std::cout);
	// Link the vertex and fragment shader into a shader program
	data = glCreateProgram();
	glAttachShader(data, vertShader);
	if (!geo.empty()) {
		glAttachShader(data, geoShader);
	}
	glAttachShader(data, fragShader);
	glLinkProgram(data);
	glDetachShader(data, vertShader);
	glDetachShader(data, fragShader);
	if (!geo.empty()) {
		glDetachShader(data, geoShader);
		glDeleteShader(geoShader);
	}
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}
}
