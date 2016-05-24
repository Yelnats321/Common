#pragma once
#include "lib.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace gl {
class Model {
	glm::mat4 modelMatrix_;
	glm::vec3 scale_, position_, posOrigin_;
	glm::quat rotation_, rotOrigin_;
	void change();
public:
	LIBRARY_API void setScale(glm::vec3 &);
	LIBRARY_API void setScale(float, float, float);
	LIBRARY_API void setRotation(glm::vec3 &);
	LIBRARY_API void setRotation(float, float, float);
	LIBRARY_API void setPosition(glm::vec3 &);
	LIBRARY_API void setPosition(float, float, float);
	LIBRARY_API const glm::mat4 & getMatrix() const;
};
}