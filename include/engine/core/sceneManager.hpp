#pragma once

#include <memory>
#include <engine/core/scene.hpp>


class SceneManager {
	void push(std::unique_ptr<Scene> scene);
	void pop();

	void update();
	void render();

	bool empty();
};