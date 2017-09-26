#pragma once

#include "message.hpp"

class chat_history {
public:

	std::vector<abstract_chat_message*> messages;

	chat_history() = default;
	chat_history(const chat_history&) = delete;
	chat_history& operator=(const chat_history&) = delete;
	chat_history(chat_history&&);
	chat_history& operator=(chat_history&&);

	~chat_history();

	void update();
	void draw();

	void add_message(const message_packet::fields& data);
	void add_image(const image_packet::fields& data);

};