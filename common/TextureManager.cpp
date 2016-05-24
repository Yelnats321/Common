#include "TextureManager.h"

extern "C"{
#include <SOIL.h>
}
namespace gl {
const gl::Texture & TextureManager::loadTexture(const std::string & name) {
	bool mipmap = true, sRGB = false;
	if (textureMap_.count(name) == 0) {
		gl::Texture texture;
		texture.gen();
		glBindTexture(GL_TEXTURE_2D, texture);
		int width, height, channels;
		unsigned char* image = SOIL_load_image(name.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
		if (sRGB)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

		if (mipmap)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 8);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
		//TODO: this doesn't work on radeon cards maybe
		glEnable(GL_TEXTURE_2D);
		glGenerateMipmap(GL_TEXTURE_2D);

		if (image == nullptr) {
			//std::cerr << " ***Error loading texture " + name + " due to " + SOIL_last_result() << std::endl;
			throw std::runtime_error("Texture load error");
		}
		SOIL_free_image_data(image);

		textureMap_.emplace(name, std::move(texture));
	}
	return textureMap_[name];
}
}