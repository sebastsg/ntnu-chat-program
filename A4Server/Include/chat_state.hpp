#pragma once

#include "engine.hpp"
#include "graphics.hpp"
#include "camera.hpp"
#include "network.hpp"
#include "socket_container.hpp"
#include "debug.hpp"
#include "a4.hpp"

class client_data {
public:

	user_info_packet::fields user;

};

class channel_data {
public:

	channel_packet::fields data;

	channel_data() = default;
	channel_data(const std::string& name) {
		data.name = name;
	}

};

class chat_state : public ne::program_state {
public:

	ne::ortho_camera camera;
	ne::font_text fps;

	ne::socket_container sockets;
	ne::connection_establisher establisher;

	std::unordered_map<size_t, client_data> clients;

	a4_userlist<user_info_packet::fields> users;
	std::unordered_map<std::string, channel_data> channels;

	void add_client_listeners(size_t index);

	chat_state();
	~chat_state();

	void update();
	void draw();

};