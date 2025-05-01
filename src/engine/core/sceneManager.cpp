#include <engine/core/sceneManager.hpp>

void SceneManager::push(std::unique_ptr<Scene> scene)
{
	scene->onEnter();
	sceneStack.push_back(std::move(scene));
}

void SceneManager::pop()
{
	sceneStack.back()->onExit();
	sceneStack.pop_back();
}

void SceneManager::update(float dt)
{
	sceneStack.back()->update(dt);
}

void SceneManager::render()
{
	sceneStack.back()->render();
}

bool SceneManager::isEmpty()
{
	return sceneStack.empty();
}
