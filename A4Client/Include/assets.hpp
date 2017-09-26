#pragma once

#include "asset.hpp"

class texture_assets : public ne::texture_group {
public:

	ne::texture blank;
	ne::texture button;

	ne::texture emojis[7];

	void initialize();

};

class font_assets : public ne::font_group {
public:

	ne::font message;
	ne::font login_input;

	void initialize();

};

class shader_assets : public ne::shader_group {
public:

	ne::shader basic;

	void initialize();

};

void create_asset_container();
void destroy_asset_container();

texture_assets& _textures();
#define textures _textures()

font_assets& _fonts();
#define fonts _fonts()

shader_assets& _shaders();
#define shaders _shaders()
