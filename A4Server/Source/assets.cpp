#include "assets.hpp"
#include "debug.hpp"
#include "engine.hpp"

class asset_container {
public:
	texture_assets _texture_assets;
	font_assets _font_assets;
	shader_assets _shader_assets;
};

asset_container* assets = nullptr;

void create_asset_container() {
	if (assets) {
		DEBUG(0, NE_WARNING, "Already exists");
		return;
	}
	assets = new asset_container();
	textures.initialize();
	fonts.initialize();
	shaders.initialize();
}

void destroy_asset_container() {
	if (!assets) {
		return;
	}
	assets->_texture_assets.finish();
	delete assets;
	assets = nullptr;
}

texture_assets& _textures() {
	return assets->_texture_assets;
}

font_assets& _fonts() {
	return assets->_font_assets;
}

shader_assets& _shaders() {
	return assets->_shader_assets;
}

void font_assets::initialize() {
	load({ &message, "consola.ttf", 22 });
}

void shader_assets::initialize() {
	load({ &basic, "basic" });
}

void texture_assets::initialize() {
	load({ &blank, "blank.png" }, false);
	spawn_thread();
	finish();
}