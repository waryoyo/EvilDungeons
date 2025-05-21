#include <engine_core/core/lightSystem.hpp>
#include <algorithm>
#include <engine_core/components/lightComponent.hpp>

void LightSystem::add(LightComponent* light)
{
	lights.push_back(light);
}

void LightSystem::remove(LightComponent* light)
{
	lights.erase(std::remove(lights.begin(), lights.end(), light), lights.end());
}
const std::vector<LightComponent*>& LightSystem::getAll() const
{
	return lights;
}

void LightSystem::clear()
{
	lights.clear();
}

void LightSystem::setDirty()
{
	dirty = true;
}

