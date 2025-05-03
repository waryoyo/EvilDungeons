#pragma once
#define GLFW_INCLUDE_NONE

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <engine/graphics/shader.hpp>
#include <engine/graphics/texture.hpp>
#include <engine/core/camera.hpp>
#include <engine/core/model.hpp>