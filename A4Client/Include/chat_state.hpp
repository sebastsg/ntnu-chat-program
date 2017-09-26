#pragma once

#include "engine.hpp"
#include "graphics.hpp"
#include "camera.hpp"
#include "network.hpp"
#include "event.hpp"
#include "ui.hpp"

#include "a4.hpp"

#include "userlist.hpp"
#include "input_box.hpp"
#include "cameras.hpp"
#include "channel.hpp"

class chat_state : public ne::program_state {
public:

	user_info_packet::fields user;

	ne::ortho_camera camera;
	ne::font_text fps;

	ne::io_socket server;

	chat_input_box input_box;

	chat_userlist userlist;
	channel_list channels;

	ne::ui_button send_image_button;

	void add_user(const user_info_packet::fields& data);

	chat_state(std::string username);
	~chat_state();

	void update();
	void draw();

};
