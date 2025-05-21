#pragma once

#include <vector>

#include <engine/utils/types.hpp>
#include <engine/components/lightComponent.hpp>

class LightSystem {
public:
	void add(LightComponent* light);
	void remove(LightComponent* light);
	const std::vector<LightComponent*>& getAll() const;
	void clear();

	void setDirty();

private:
	std::vector<LightComponent*> lights;
	bool dirty = false;
};