#pragma once

#include "a4.hpp"
#include "graphics.hpp"

class client_user : public user_info_packet::fields {
public:
	
	ne::transform3f status_transform;
	ne::font_text label;

};

class chat_userlist {
public:

	a4_userlist<client_user> list;

	ne::transform3f transform;

	void update();
	void draw();

};