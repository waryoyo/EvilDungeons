#pragma once

class Scene {
public:
	virtual ~Scene() = default;
	virtual void onEnter() = 0;
	virtual void onExit() = 0;

	virtual void update(float deltaTime) = 0;
	virtual void render() = 0;

};