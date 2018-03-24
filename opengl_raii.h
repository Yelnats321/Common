#pragma once

#include <glad/glad.h>

#include <array>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace gl {
namespace detail {
    template <size_t Count>
    class opengl_resource {
    public:
        static_assert(Count > 0, "Can't have 0 sized resources");

        opengl_resource() = default;

        opengl_resource(opengl_resource&& o) noexcept {
            std::swap(resource, o.resource);
        }
        opengl_resource& operator=(opengl_resource&& o) noexcept {
            std::swap(resource, o.resource);
            return *this;
        }

        GLuint operator[](size_t idx) const {
            return resource[idx];
        }

        operator GLuint() const {
            static_assert(Count == 1,
                          "Don't use converting operator for resources with more than 1 count");
            return resource[0];
        }

    protected:
        std::array<GLuint, Count> resource{};
    };

    template <GLenum Type, size_t Count>
    class texture_array_base : public opengl_resource<Count> {
        using base = opengl_resource<Count>;

    public:
        texture_array_base() {
            glCreateTextures(Type, Count, this->resource.data());
        }

        ~texture_array_base() {
            glDeleteTextures(Count, this->resource.data());
        }

        texture_array_base(texture_array_base&& other) noexcept : base(std::move(other)) {}
        texture_array_base& operator=(texture_array_base&& other) noexcept {
            base::operator=(std::move(other));
            return *this;
        }

        void parameteri(GLenum pname, GLint param, size_t idx = 0) const {
            glTextureParameteri(this->resource[idx], pname, param);
        }

        void parameterfv(GLenum pname, const GLfloat* param, size_t idx = 0) const {
            glTextureParameterfv(this->resource[idx], pname, param);
        }

        void bind(GLuint first = 0) const {
            glBindTextures(first, Count, this->resource.data());
        }
    };
} // namespace detail

template <GLenum Type, size_t Count>
class buffer_array : public detail::opengl_resource<Count> {
    using base = detail::opengl_resource<Count>;

public:
    buffer_array() {
        glCreateBuffers(Count, this->resource.data());
    }
    ~buffer_array() {
        glDeleteBuffers(Count, this->resource.data());
    }

    buffer_array(buffer_array&& other) noexcept : base(std::move(other)) {}
    buffer_array& operator=(buffer_array&& other) noexcept {
        base::operator=(std::move(other));
        return *this;
    }

    void storage(GLsizeiptr size, const void* data, GLbitfield flags = 0, size_t idx = 0) const {
        glNamedBufferStorage(this->resource[idx], size, data, flags);
    }
    void subData(GLintptr offset, GLsizeiptr size, const void* data, size_t idx = 0) const {
        glNamedBufferSubData(this->resource[idx], offset, size, data);
    }
    void data(GLsizei size, const void* data, GLenum usage, size_t idx = 0) const {
        glNamedBufferData(this->resource[idx], size, data, usage);
    }
};

template <GLenum Type>
using buffer = buffer_array<Type, 1>;

template <size_t Count>
class vertex_array_array : public detail::opengl_resource<Count> {
    using base = detail::opengl_resource<Count>;

public:
    vertex_array_array() {
        glCreateVertexArrays(Count, this->resource.data());
    }
    ~vertex_array_array() {
        glDeleteVertexArrays(Count, this->resource.data());
    }

    vertex_array_array(vertex_array_array&& other) noexcept : base(std::move(other)) {}
    vertex_array_array& operator=(vertex_array_array&& other) noexcept {
        base::operator=(std::move(other));
        return *this;
    }

    void bind(size_t idx = 0) const {
        glBindVertexArray(this->resource[idx]);
    }
    void vertexBuffer(GLuint bindingindex, const buffer<GL_ARRAY_BUFFER>& buffer, GLintptr offset,
                      GLsizei stride, size_t idx = 0) const {
        glVertexArrayVertexBuffer(this->resource[idx], bindingindex, buffer, offset, stride);
    }
    void enableAttrib(GLuint index, size_t idx = 0) const {
        glEnableVertexArrayAttrib(this->resource[idx], index);
    }
    void attribFormat(GLuint attribindex, GLint size, GLenum type, GLboolean normalized,
                      GLuint relativeoffset, size_t idx = 0) const {
        glVertexArrayAttribFormat(this->resource[idx], attribindex, size, type, normalized,
                                  relativeoffset);
    }
    void attribBinding(GLuint attribindex, GLuint bindingindex, size_t idx = 0) const {
        glVertexArrayAttribBinding(this->resource[idx], attribindex, bindingindex);
    }
    void elementBuffer(const buffer<GL_ELEMENT_ARRAY_BUFFER>& buffer, size_t idx = 0) const {
        glVertexArrayElementBuffer(this->resource[idx], buffer);
    }
};

using vertex_array = vertex_array_array<1>;

template <GLenum Type>
class shader : public detail::opengl_resource<1> {
    using base = detail::opengl_resource<1>;

public:
    shader() {
        this->resource[0] = glCreateShader(Type);
    }
    ~shader() {
        glDeleteShader(this->resource[0]);
    }

    shader(shader&& other) noexcept : base(std::move(other)) {}
    shader& operator=(shader&& other) noexcept {
        base::operator=(std::move(other));
        return *this;
    }

    void source(GLsizei count, const GLchar* const* string, const GLint* length = nullptr) const {
        glShaderSource(this->resource[0], count, string, length);
    }
    void source_from_file(const char* fileName) const {
        std::ifstream file(fileName);
        if (!file) {
            std::cerr << "***No file named " << fileName << "\n";
            throw "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();

        auto bufStr = buffer.str();
        auto bufCStr = bufStr.c_str();
        source(1, &bufCStr);
    }
    void compile() const {
        glCompileShader(this->resource[0]);
        GLint success = 0;
        glGetShaderiv(this->resource[0], GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            GLint logSize = 0;
            glGetShaderiv(this->resource[0], GL_INFO_LOG_LENGTH, &logSize);
            std::vector<GLchar> log(logSize);
            glGetShaderInfoLog(this->resource[0], logSize, nullptr, log.data());
            std::cout << "***Shader compile error\n";
            std::cerr << log.data() << "\n";
        }
    }
};

class program : public detail::opengl_resource<1> {
    using base = detail::opengl_resource<1>;

public:
    program() {
        this->resource[0] = glCreateProgram();
    }
    ~program() {
        glDeleteProgram(this->resource[0]);
    }

    program(program&& other) noexcept : base(std::move(other)) {}
    program& operator=(program&& other) noexcept {
        base::operator=(std::move(other));
        return *this;
    }

    template <GLenum... ShaderTypes>
    void attach(const shader<ShaderTypes>&... shaders) const {
        std::initializer_list<int> _{((void)glAttachShader(this->resource[0], shaders), 0)...};
    }
    template <GLenum... ShaderTypes>
    void detach(const shader<ShaderTypes>&... shaders) const {
        std::initializer_list<int> _{((void)glDetachShader(this->resource[0], shaders), 0)...};
    }
    void link() const {
        glLinkProgram(this->resource[0]);
    }
    void use() const {
        glUseProgram(this->resource[0]);
    }
    void uniform1i(GLint location, GLint v0) const {
        glProgramUniform1i(this->resource[0], location, v0);
    }
    void uniform3fv(GLint location, GLsizei count, const GLfloat* value) const {
        glProgramUniform3fv(this->resource[0], location, count, value);
    }
    void uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose,
                          const GLfloat* value) const {
        glProgramUniformMatrix4fv(this->resource[0], location, count, transpose, value);
    }
};

template <GLenum Type, size_t Count>
class texture_array;

template <size_t Count>
class texture_array<GL_TEXTURE_2D, Count>
    : public detail::texture_array_base<GL_TEXTURE_2D, Count> {
    using base = detail::texture_array_base<GL_TEXTURE_2D, Count>;

public:
    texture_array() = default;

    texture_array(texture_array&& other) noexcept : base(std::move(other)) {}
    texture_array& operator=(texture_array&& other) noexcept {
        base::operator=(std::move(other));
        return *this;
    }

    void storage(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height,
                 size_t idx = 0) const {
        glTextureStorage2D(this->resource[idx], levels, internalformat, width, height);
    }
    void subImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                  GLenum format, GLenum type, const void* pixels, size_t idx = 0) const {
        glTextureSubImage2D(this->resource[idx], level, xoffset, yoffset, width, height, format,
                            type, pixels);
    }
};

template <GLenum Type>
using texture = texture_array<Type, 1>;

template <GLenum Target, size_t Count>
class framebuffer_array : public detail::opengl_resource<Count> {
    using base = detail::opengl_resource<Count>;

public:
    framebuffer_array() {
        glCreateFramebuffers(Count, this->resource.data());
    }
    ~framebuffer_array() {
        glDeleteFramebuffers(Count, this->resource.data());
    }

    framebuffer_array(framebuffer_array&& other) noexcept : base(std::move(other)) {}
    framebuffer_array& operator=(framebuffer_array&& other) noexcept {
        base::operator=(std::move(other));
        return *this;
    }

    void texture(GLenum attachment, GLuint texture, GLint level, size_t idx = 0) const {
        glNamedFramebufferTexture(this->resource[idx], attachment, texture, level);
    }
    void drawBuffer(GLenum buf, size_t idx = 0) const {
        glNamedFramebufferDrawBuffer(this->resource[idx], buf);
    }
    void readBuffer(GLenum buf, size_t idx = 0) const {
        glNamedFramebufferReadBuffer(this->resource[idx], buf);
    }
    void bind(size_t idx = 0) const {
        glBindFramebuffer(Target, this->resource[idx]);
    }
};

template <GLenum Target>
using framebuffer = framebuffer_array<Target, 1>;

} // namespace gl
