#pragma once
#include "lib.h"
#include <GL/glew.h>
#include <string>

#define MAKE_OGL_WRAPPER(gen, del) OGLWrapper<decltype(&gen), &gen, decltype(&del), &del>

namespace gl {
template<class TB, TB TCons, class TF, TF TDes>
class OGLWrapper{
protected:
	GLuint data = 0;
public:
	LIBRARY_API OGLWrapper() {}
	LIBRARY_API OGLWrapper(const OGLWrapper&) = delete;
	LIBRARY_API OGLWrapper& operator=(const OGLWrapper&) = delete;
	LIBRARY_API OGLWrapper(OGLWrapper && rhs) {
		std::swap(data, rhs.data);
	}
	LIBRARY_API OGLWrapper& operator=(OGLWrapper&& rhs) {
		std::swap(data, rhs.data);
		return *this;
	}
	LIBRARY_API ~OGLWrapper() {
		(*TDes)(1, &data);
	}
	LIBRARY_API void gen() {
		(*TCons)(1, &data);
	}
	LIBRARY_API inline operator GLuint() const{ return data; }
};

using VAO = MAKE_OGL_WRAPPER(glGenVertexArrays, glDeleteVertexArrays);
using Buffer = MAKE_OGL_WRAPPER(glGenBuffers, glDeleteBuffers);
using Renderbuffer = MAKE_OGL_WRAPPER(glGenRenderbuffers, glDeleteRenderbuffers);
using Framebuffer = MAKE_OGL_WRAPPER(glGenFramebuffers, glDeleteFramebuffers);

struct Texture : MAKE_OGL_WRAPPER(glGenTextures, glDeleteTextures) {
public:
	//void gen(string, bool = false, bool = true);
};

class Program{
	GLuint data = 0;
public:
	LIBRARY_API Program();
	LIBRARY_API Program(Program &&);
	LIBRARY_API Program(const Program&) = delete;
	LIBRARY_API Program& operator=(const Program&) = delete;
	LIBRARY_API Program& operator=(Program&& rhs);
	LIBRARY_API ~Program();
	LIBRARY_API void gen(std::string v, std::string f, std::string g = "");
	LIBRARY_API inline operator GLuint() const{ return data; }
};
}
