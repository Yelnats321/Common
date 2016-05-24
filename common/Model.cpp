#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>

namespace gl {
void Model::change() {
	modelMatrix_ = glm::mat4(1.0);
	modelMatrix_ = glm::translate(modelMatrix_, position_);
	modelMatrix_ *= glm::mat4_cast(rotation_ * rotOrigin_);
	modelMatrix_ = glm::scale(modelMatrix_, scale_);
	modelMatrix_ = glm::translate(modelMatrix_, -posOrigin_);
}

void Model::setRotation(glm::vec3 & rot) {
	rotation_ = glm::quat(rot);
	change();
}
void Model::setRotation(float x, float y, float z) {
	setRotation(glm::vec3(x, y, z));
}

void Model::setScale(glm::vec3 & _scale) {
	scale_ = _scale;
	change();
}
void Model::setScale(float x, float y, float z) {
	setScale(glm::vec3(x, y, z));
}

void Model::setPosition(glm::vec3 & pos) {
	position_ = pos;
	change();
}
void Model::setPosition(float x, float y, float z) {
	setPosition(glm::vec3(x, y, z));
}
const glm::mat4 & Model::getMatrix() const {
	return modelMatrix_;
}
}