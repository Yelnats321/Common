#pragma once
#include "lib.h"
#include "OGLWrapper.h"
#include <string>
#include <unordered_map>

namespace gl {
class TextureManager {
	std::unordered_map<std::string, gl::Texture> textureMap_;
public:
	LIBRARY_API const gl::Texture & loadTexture(const std::string &);
};
}