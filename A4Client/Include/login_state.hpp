#pragma once

#include "engine.hpp"
#include "input_box.hpp"
#include "cameras.hpp"

class login_state : public ne::program_state {
public:

	ne::ortho_camera camera;

	ne::font_text login_label;

	chat_input_box username;

	std::string username_submitted;

	login_state();
	~login_state();

	void update();
	void draw();

};