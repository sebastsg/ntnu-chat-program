#pragma once

#include "a4.hpp"
#include "graphics.hpp"

class chat_emoji {
public:

	ne::transform3f transform;
	ne::texture* texture = nullptr;
	std::string identifier;

};

class chat_message_part {
public:

	ne::font_text label;
	bool is_link = false;

};

class abstract_chat_message {
public:

	virtual uint32 timestamp() const {
		return 0;
	}

	virtual std::string sent_by() const {
		return "";
	}

	ne::font_text sent_by_label;
	ne::font_text time_label;

	ne::transform3f transform;

	bool hide_header = false;
	float body_height = 0.0f;
	bool show_separator = false;
	
	virtual void render() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;

};

class chat_message : public abstract_chat_message {
public:

	message_packet::fields data;

	uint32 timestamp() const {
		return data.timestamp;
	}

	std::string sent_by() const {
		return data.sent_by;
	}

	std::vector<chat_message_part> body;
	std::vector<chat_emoji> emojis;

	void render();
	void update();
	void draw();

};

class chat_image : public abstract_chat_message {
public:

	image_packet::fields data;

	uint32 timestamp() const {
		return data.timestamp;
	}

	std::string sent_by() const {
		return data.sent_by;
	}

	ne::texture image;

	void render();
	void update();
	void draw();

};
