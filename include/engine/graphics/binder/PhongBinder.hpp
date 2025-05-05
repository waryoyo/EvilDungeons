#pragma once

#include <engine/graphics/binder/IUniformBinder.hpp>
#include <engine/utils/types.hpp>

#include <vector>

class PhongBinder : public IUniformBinder {
public:
	PhongBinder(Material material) : material(material) {}
	void apply(const BinderParams& params) const override;

   
private:
	Material material;
};