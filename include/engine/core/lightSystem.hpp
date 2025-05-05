#pragma once

#include <vector>

class LightComponent;

class LightSystem {
public:
	void add(LightComponent* light);
	void remove(LightComponent* light);
	const std::vector<LightComponent*>& getAll() const;
	void clear();
private:
	std::vector<LightComponent*> lights;
};