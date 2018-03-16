#pragma once

#include <array>
#include <vector>
#include <initializer_list>

namespace gl {
namespace detail {
template <typename Trait, size_t Count>
class opengl_resource {
	static_assert(Count > 0, "Can't have 0 sized resources");
	Trait * asTrait() { return static_cast<Trait*>(this); }
protected:
	std::array<GLuint, Count> resource{};
public:
	opengl_resource() = default;
	opengl_resource(opengl_resource &&o) {
		std::swap(resource, o.resource);
	}
	opengl_resource & operator=(opengl_resource &&o) {
		std::swap(resource, o.resource);
		return *this;
	}
	~opengl_resource() {
		del();
	}

	void create() {
		del();
		asTrait()->create_impl();
	}
	void del() {
		asTrait()->del_impl();
		resource.fill(0);
	}
	GLuint operator[](size_t idx) const {
		return resource[idx];
	}
	operator GLuint() const {
		static_assert(Count == 1,
					  "Don't use converting operator for resources with more than 1 count");
		return resource[0];
	}
};

template <GLenum Type, size_t Count>
class texture_array_base: public opengl_resource<texture_array_base<Type, Count>, Count> {
	friend class opengl_resource<texture_array_base, Count>;
	void create_impl() {
		glCreateTextures(Type, Count, resource.data());
	}
	void del_impl() {
		glDeleteTextures(Count, resource.data());
	}
public:
	void parameteri(GLenum pname, GLint param, size_t idx = 0) const {
		glTextureParameteri(resource[idx], pname, param);
	}
	void parameterfv(GLenum pname, const GLfloat *param, size_t idx = 0) const {
		glTextureParameterfv(resource[idx], pname, param);
	}
	void bind(GLuint first) const {
		glBindTextures(first, Count, resource.data());
	}
};
} // namespace detail

template <GLenum Type, size_t Count>
class buffer_array: public detail::opengl_resource<buffer_array<Type, Count>, Count> {
	friend class detail::opengl_resource<buffer_array<Type, Count>, Count>;
	void create_impl() {
		glCreateBuffers(Count, resource.data());
	}
	void del_impl() {
		glDeleteBuffers(Count, resource.data());
	}
public:
	void storage(GLsizeiptr size, const void * data, GLbitfield flags, size_t idx = 0) const {
		glNamedBufferStorage(resource[idx], size, data, flags);
	}
	void subData(GLintptr offset, GLsizeiptr size, const void *data, size_t idx = 0) const {
		glNamedBufferSubData(resource[idx], offset, size, data);
	}
};

template <size_t Count>
class vertex_array_array: public detail::opengl_resource<vertex_array_array<Count>, Count> {
	friend class detail::opengl_resource<vertex_array_array<Count>, Count>;
	void create_impl() {
		glCreateVertexArrays(Count, resource.data());
	}
	void del_impl() {
		glDeleteVertexArrays(Count, resource.data());
	}
public:
	void bind(size_t idx = 0) const {
		glBindVertexArray(resource[idx]);
	}
	void vertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride, size_t idx = 0) const {
		glVertexArrayVertexBuffer(resource[idx], bindingindex, buffer, offset, stride);
	}
	void enableAttrib(GLuint index, size_t idx = 0) const {
		glEnableVertexArrayAttrib(resource[idx], index);
	}
	void attribFormat(GLuint attribindex, GLint size, GLenum type, GLboolean normalized,
					  GLuint relativeoffset, size_t idx = 0) const {
		glVertexArrayAttribFormat(resource[idx], attribindex, size, type, normalized, relativeoffset);
	}
	void attribBinding(GLuint attribindex, GLuint bindingindex, size_t idx = 0) const {
		glVertexArrayAttribBinding(resource[idx], attribindex, bindingindex);
	}
	void elementBuffer(GLuint buffer, size_t idx = 0) const {
		glVertexArrayElementBuffer(resource[idx], buffer);
	}
};

template <GLenum Type>
class shader: public detail::opengl_resource<shader<Type>, 1> {
	friend class detail::opengl_resource<shader<Type>, 1>;
	void create_impl() {
		resource[0] = glCreateShader(Type);
	}
	void del_impl() {
		glDeleteShader(resource[0]);
	}
public:
	void source(GLsizei count, const GLchar *const *string, const GLint *length) const {
		glShaderSource(resource[0], count, string, length);
	}
	void sourceFromFile(const char *fileName) const {
		std::ifstream file(fileName);
		if (!file) {
			std::cerr << "No file named " << fileName << "\n";
			throw "";
		}
		std::stringstream buffer;
		buffer << file.rdbuf();

		auto bufStr = buffer.str();
		auto bufCStr = bufStr.c_str();
		source(1, &bufCStr, nullptr);
	}
	void compile() const {
		glCompileShader(resource[0]);
		GLint success = 0;
		glGetShaderiv(resource[0], GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE) {
			GLint logSize = 0;
			glGetShaderiv(resource[0], GL_INFO_LOG_LENGTH, &logSize);
			std::vector<GLchar> log(logSize);
			glGetShaderInfoLog(resource[0], logSize, nullptr, log.data());
			std::cerr << log.data() << "\n";
		}
	}
};

class program: public detail::opengl_resource<program, 1> {
	friend class detail::opengl_resource<program, 1>;
	void create_impl() {
		resource[0] = glCreateProgram();
	}
	void del_impl() {
		glDeleteProgram(resource[0]);
	}
public:
	template <GLenum... ShaderTypes>
	void attach(const shader<ShaderTypes>&... shaders) const {
		std::initializer_list<int> _{((void)glAttachShader(resource[0], shaders), 0)...};
	}
	template <GLenum... ShaderTypes>
	void detach(const shader<ShaderTypes>&... shaders) const {
		std::initializer_list<int> _{((void)glDetachShader(resource[0], shaders), 0)...};
	}
	void link() const {
		glLinkProgram(resource[0]);
	}
	void use() const {
		glUseProgram(resource[0]);
	}
	void uniform1i(GLint location, GLint v0) const {
		glProgramUniform1i(resource[0], location, v0);
	}
	void uniform3fv(GLint location, GLsizei count, const GLfloat *value) const {
		glProgramUniform3fv(resource[0], location, count, value);
	}
	void uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) const {
		glProgramUniformMatrix4fv(resource[0], location, count, transpose, value);
	}
};

template <GLenum Type, size_t Count>
class texture_array;

template <size_t Count>
class texture_array<GL_TEXTURE_2D, Count>: public detail::texture_array_base<GL_TEXTURE_2D, Count> {
public:
	void storage(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, size_t idx = 0) const {
		glTextureStorage2D(resource[idx], levels, internalformat, width, height);
	}
	void subImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
				  GLenum format, GLenum type, const void *pixels, size_t idx = 0) const {
		glTextureSubImage2D(resource[idx], level, xoffset, yoffset,
							width, height, format, type, pixels);
	}
};

template <GLenum Target, size_t Count>
class framebuffer_array: public detail::opengl_resource<framebuffer_array<Target, Count>, Count> {
	friend class detail::opengl_resource<framebuffer_array<Target, Count>, Count>;
	void create_impl() {
		glCreateFramebuffers(Count, resource.data());
	}
	void del_impl() {
		glDeleteFramebuffers(Count, resource.data());
	}
public:
	void texture(GLenum attachment, GLuint texture, GLint level, size_t idx = 0) const {
		glNamedFramebufferTexture(resource[idx], attachment, texture, level);
	}
	void drawBuffer(GLenum buf, size_t idx = 0) const {
		glNamedFramebufferDrawBuffer(resource[idx], buf);
	}
	void readBuffer(GLenum buf, size_t idx = 0) const {
		glNamedFramebufferReadBuffer(resource[idx], buf);
	}
	void bind(size_t idx = 0) const {
		glBindFramebuffer(Target, resource[idx]);
	}
};

template <GLenum Type>
using buffer = buffer_array<Type, 1>;

using vertex_array = vertex_array_array<1>;

template <GLenum Type>
using texture = texture_array<Type, 1>;

template <GLenum Target>
using framebuffer = framebuffer_array<Target, 1>;

} // namespace gl
