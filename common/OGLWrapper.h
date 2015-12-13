#pragma once
#include <GL/glew.h>
#include <string>

#define MAKE_OGL_WRAPPER(gen, del) OGLWrapper<decltype(&gen), &gen, decltype(&del), &del>

//class ObjFile;
namespace gl{
template<class TB, TB TCons, class TF, TF TDes>
class OGLWrapper{
protected:
	GLuint data = 0;
public:
	OGLWrapper() {} 
	OGLWrapper(const OGLWrapper&) = delete;
	OGLWrapper& operator=(const OGLWrapper&) = delete;
	OGLWrapper(OGLWrapper && rhs) {
		std::swap(data, rhs.data);
	}
	OGLWrapper& operator=(OGLWrapper&& rhs) {
		std::swap(data, rhs.data);
		return *this;
	}
	~OGLWrapper() {
		(*TDes)(1, &data);
	}
	void gen() {
		(*TCons)(1, &data);
	}
	inline operator GLuint() const{ return data; }
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
	Program();
	Program(Program &&);
	Program(const Program&) = delete;
	Program& operator=(const Program&) = delete;
	Program& operator=(Program&& rhs);
	~Program();
	void gen(std::string v, std::string f, std::string g = "");
	inline operator GLuint() const{ return data; }
};
}
