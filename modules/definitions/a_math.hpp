#pragma once
#include <glm/glm.hpp>          
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};