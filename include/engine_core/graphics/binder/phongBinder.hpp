#pragma once

#include <engine_core/graphics/binder/IUniformBinder.hpp>
#include <engine_core/utils/types.hpp>

#include <vector>

class PhongBinder : public IUniformBinder {
public:
	PhongBinder(Material material) : material(material) {}
	void apply(const BinderParams& params) const override;

   
private:
	Material material;
};
