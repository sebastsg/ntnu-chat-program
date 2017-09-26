#pragma once

#include "a4.hpp"
#include "graphics.hpp"

class chat_input_box {
public:

	struct submit_message {
		std::string message;
	};

	ne::font* font = nullptr;

	ne::event_listener<ne::message_event, submit_message> submit;

	ne::transform3f transform;

	int key_listener = -1;
	int input_listener = -1;
	int click_listener = -1;

	std::vector<std::string> input;
	std::vector<ne::font_text> input_text;
	int line = 0;
	int column = 0;

	bool allow_multiline = true;

	struct {

		int from_line = -1;
		int from_column = -1;
		int to_line = -1;
		int to_column = -1;

		void reset() {
			from_line = -1;
			from_column = -1;
			to_line = -1;
			to_column = -1;
		}

		bool selected_line(int line) {
			return line >= from_line && line <= to_line;
		}

	} selection;

	bool is_active = true;

	chat_input_box();
	~chat_input_box();

	void update();
	void draw();

	bool erase_selection();
	void shift_selection_left(int* from_column, int* from_line, int* to_column, int* to_line);
	void shift_selection_right(int* from_column, int* from_line, int* to_column, int* to_line);

};
