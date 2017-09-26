#include "input_box.hpp"
#include "assets.hpp"
#include "engine.hpp"
#include "SDL/SDL.h"

bool chat_input_box::erase_selection() {

	// Do we have a selection?
	if (selection.from_line == -1 || selection.from_column == -1 || selection.to_line == -1 || selection.to_column == -1) {
		return false;
	}

	// Is the selection on the same line?
	if (selection.from_line == selection.to_line) {

		// Yes, then it's quite easy to just remove those characters.
		input[selection.from_line].erase(selection.from_column, selection.to_column - selection.from_column);

		// Update column, so it doesn't go out of bounds.
		column = selection.from_column;

		// Clear the selection.
		selection.reset();

		return true;

	}

	// Remove characters in first possibly partly-selected line.
	input[selection.from_line].erase(selection.from_column, input[selection.from_line].size() - selection.from_column);

	// Update column, so it doesn't go out of bounds.
	column = selection.from_column;
	
	// Remove characters in fully selected lines.
	for (int i = selection.from_line + 1; i < selection.to_line; i++) {
		if (input.size() > i) {
			input[i].clear();
		}
	}

	// Remove characters in last possibly part-selected line.
	input[selection.to_line].erase(0, selection.to_column);

	// Erase all empty lines.
	for (size_t i = 0; i < input.size(); i++) {
		if (input[i].size() == 0) {
			input.erase(input.begin() + i);
			i--;
		}
	}

	// Make sure we have at least one line.
	if (input.size() == 0) {
		input.push_back("");
	}

	// Clear the labels.
	input_text.clear();
	for (auto& i : input) {
		input_text.push_back({});
	}

	// Clear the selection.
	selection.reset();

	// Make sure the line is not out of bounds.
	if (line >= (int)input.size()) {
		line = (int)input.size() - 1;
	}

	return true;

}

void chat_input_box::shift_selection_left(int* from_column, int* from_line, int* to_column, int* to_line) {

	// Check if we have a selection from before.
	if (*from_column == -1) {

		// Nope, let's initialise one.
		*from_column = column - 1;
		*from_line = line;
		*to_column = column;
		*to_line = line;

	} else {

		// Yes, let's go left.
		(*from_column)--;

	}

	// Did we go too far left?
	if (*from_column < 0) {

		// Yes, let's try to go upwards.
		*from_column = 0;
		(*from_line)--;

		// Did we go too far up?
		if (*from_line < 0) {

			// Yep, so let's just stay at the current line and column.
			*from_line = 0;

		} else {

			// Reached valid line, so we'll set the column at the end of it.
			*from_column = input[*from_line].size();

		}
	}

}

void chat_input_box::shift_selection_right(int* from_column, int* from_line, int* to_column, int* to_line) {

	// Check if we have a selection from before.
	if (*from_column == -1) {

		// Nope, let's initialise one.
		*from_column = column;
		*from_line = line;
		*to_column = column + 1;
		*to_line = line;

	} else {

		// Yes, let's go right.
		(*to_column)++;

	}

	// Did we go too far right?
	if (*to_column > input[*to_line].size()) {

		// Yes, let's try to go downwards.
		*to_column = input[*to_line].size();
		(*to_line)++;

		// Did we go too far down?
		if (*to_line >= input.size()) {

			// Yep, so let's just stay at the last line and column.
			*to_line = input.size() - 1;

		} else {

			// Reached valid line, so we'll set the column at the beginning of it.
			*to_column = 0;

		}

	}

}

chat_input_box::chat_input_box() {

	input.push_back("");
	input_text.push_back({});

	key_listener = ne::listen([this](ne::keyboard_key_message key) {

		if (!is_active) {
			return;
		}

		if (!key.is_pressed) {
			return;
		}

		switch (key.key) {

		case KEY_BACKSPACE:

			// Did we press backspace?
			if (key.key != KEY_BACKSPACE) {
				break;
			}

			// If we have a selection, we want to prioritise deleting that first.
			if (erase_selection()) {
				break;
			}

			// Delete one character.
			if (input[line].size() > 0) {
				if (column > 0) {
					column--;
					input[line].erase(input[line].begin() + column);
				}
			} else {
				if (input.size() > 0 && line > 0) {
					input.erase(input.begin() + line);
					input_text.erase(input_text.begin() + line);
					line--;
				}
			}

			break;

		case KEY_LEFT:
			if (SDL_GetModState() & KMOD_SHIFT) {
				if (column == selection.from_column) {
					shift_selection_left(&selection.from_column, &selection.from_line, &selection.to_column, &selection.to_line);
				} else {
					shift_selection_left(&selection.to_column, &selection.to_line, &selection.from_column, &selection.from_line);
				}
			} else {
				selection.reset();
			}
			if (line > 0) {
				if (column > 0) {
					column--;
				} else {
					line--;
					column = input[line].size();
				}
			} else {
				if (column > 0) {
					column--;
				}
			}
			break;

		case KEY_RIGHT:
			if (SDL_GetModState() & KMOD_SHIFT) {
				if (column == selection.to_column) {
					shift_selection_right(&selection.from_column, &selection.from_line, &selection.to_column, &selection.to_line);
				} else {
					shift_selection_right(&selection.to_column, &selection.to_line, &selection.from_column, &selection.from_line);
				}
			} else {
				selection.reset();
			}
			if (line == 0) {
				if ((int)input[0].size() > column) {
					column++;
				} else if ((int)input.size() > line + 1) {
					line++;
					column = 0;
				}
			} else {
				if ((int)input[line].size() > column) {
					column++;
				} else if ((int)input.size() > line + 1) {
					line++;
					column = 0;
				}
			}
			break;

		case KEY_UP:
			selection.reset();
			if (line > 0) {
				line--;
				if (column > (int)input[line].size()) {
					column = (int)input[line].size();
				}
			}
			break;

		case KEY_DOWN:
			selection.reset();
			if ((int)input.size() > line + 1) {
				line++;
				if (column > (int)input[line].size()) {
					column = (int)input[line].size();
				}
			}
			break;

		case KEY_RETURN:
			if (input.size() == 1 && input[0].size() == 0) {
				break;
			}
			if (allow_multiline && (SDL_GetModState() & KMOD_SHIFT)) {
				line++;
				column = 0;
			} else {
				chat_input_box::submit_message submit_message;
				for (auto& i : input) {
					submit_message.message += i + "\n";
				}
				submit_message.message.erase(submit_message.message.end() - 1);
				submit.trigger(submit_message);
				selection.reset();
				line = 0;
				column = 0;
				input.clear();
				input_text.clear();
			}
			input.push_back("");
			input_text.push_back({});
			break;

		case KEY_A:
			if (SDL_GetModState() & KMOD_CTRL) {
				selection.from_line = 0;
				selection.from_column = 0;
				selection.to_line = input.size() - 1;
				selection.to_column = input[line].size();
			}
			break;

		default:
			break;

		}

	});

	input_listener = ne::listen([this](ne::keyboard_input_message message) {

		if (!is_active) {
			return;
		}

		erase_selection();

		for (auto& i : message.input) {
			input[line].insert(input[line].begin() + column, i);
			column++;
		}

	});

	click_listener = ne::listen([this](ne::mouse_button_message button) {

		if (!button.is_pressed || button.button != MOUSE_BUTTON_LEFT) {
			return;
		}

		is_active = transform.collides_with(ne::mouse_position_f());

	});

}

chat_input_box::~chat_input_box() {
	ne::erase(&key_listener);
	ne::erase(&input_listener);
	ne::erase(&click_listener);
}

void chat_input_box::update() {
	ne::set_font(font);
	for (size_t i = 0; i < input.size(); i++) {
		input_text[i].render(input[i]);
		input_text[i].wrap_length = (int)transform.scale.width - 24;
	}
}

void chat_input_box::draw() {

	// Draw background.
	ne::set_model_matrix(&transform);
	ne::set_texture(&textures.blank);
	if (is_active) {
		ne::push_color(72.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f, 1.0f);
	} else {
		ne::push_color(72.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f, 0.75f);
	}
	ne::draw_vertices();
	ne::pop_color();

	// Draw current text input.
	ne::push_color(1.0f, 1.0f, 1.0f, 1.0f);
	float y = 0.0f;
	for (size_t i = 0; i < input_text.size(); i++) {

		if (input[i].size() == 0) {
			y += input_text[i].transform.scale.height + 4.0f;
			continue;
		}

		input_text[i].transform.position.x = transform.position.x + 8.0f;
		input_text[i].transform.position.y = transform.position.y + 8.0f + y;

		// Is this line in the current selection?
		if (selection.selected_line(i)) {

			// Assume the line is fully selected at first.
			ne::transform3f select_transform = input_text[i].transform;

			// Is this line the first line of the selection?
			if (i == selection.from_line) {

				// Offset x by columns selected.
				select_transform.position.x += (float)(selection.from_column) * 10.0f;

				// Is the selection only one line?
				if (selection.from_line == selection.to_line) {

					// Yes, the width is the difference between the indices then.
					select_transform.scale.width = (float)(selection.to_column - selection.from_column) * 10.0f;

				} else {

					// No, this means the width is the difference between columns in this line and the selected columns.
					select_transform.scale.width = (float)(input[selection.from_line].size() - selection.from_column) * 10.0f;

				}

				// Is this line the last line of the selection?
			} else if (i == selection.to_line) {

				// Yes, the width is the selected column count.
				select_transform.scale.width = (float)selection.to_column * 10.0f;

			}

			// Draw the selection.
			ne::set_model_matrix(&select_transform);
			ne::set_texture(&textures.blank);
			ne::push_color(0.25f, 0.25f, 1.0f, 1.0f);
			ne::draw_vertices();
			ne::pop_color();

		}

		// Draw text
		if (i == line) {

			// Draw first part of the line.
			if (column != 0) {
				input_text[i].text = input[i].substr(0, column);
				input_text[i].draw();
			} else {
				input_text[i].transform.scale.width = 0.0f;
			}

			// Draw cursor.
			ne::transform3f cursor = transform;
			cursor.position.x = input_text[line].transform.position.x + input_text[line].transform.scale.width;
			cursor.position.y = input_text[line].transform.position.y;
			cursor.scale.width = 1.0f;
			cursor.scale.height = input_text[line].transform.scale.height;
			ne::set_model_matrix(&cursor);
			ne::set_texture(&textures.blank);
			ne::push_color(1.0f, 1.0f, 1.0f, 0.9f);
			ne::draw_vertices();
			ne::pop_color();

			// Draw remaining part of the line.
			if (column != input[i].size()) {
				float w = input_text[i].transform.scale.width;
				input_text[i].text = input[i].substr(column);
				input_text[i].transform.position.x += input_text[i].transform.scale.width;
				input_text[i].draw();
				input_text[i].transform.scale.width += w;
			}

		} else {

			input_text[i].draw();

		}

		y += input_text[i].transform.scale.height + 4.0f;
	}

}