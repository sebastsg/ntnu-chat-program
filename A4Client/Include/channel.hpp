#pragma once

#include "history.hpp"
#include "ui.hpp"

class chat_channel {
public:

	std::string name;
	ne::ui_button button;

	chat_history history;

	void update();
	void draw();

};

class channel_list {
public:

	ne::transform3f transform;

	std::string active_channel;
	std::unordered_map<std::string, chat_channel> channels;

	void update();
	void draw();

	void add_message(const message_packet::fields& data);
	void add_image(const image_packet::fields& data);
	void add_channel(const channel_packet::fields& data);

	void load_channel(const std::string& name);

};
