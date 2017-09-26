#include "message.hpp"
#include "assets.hpp"
#include "debug.hpp"
#include "platform.hpp"

struct message_emoji_index {

	size_t emoji_index = 0;
	size_t position = std::string::npos;

};

void chat_message::render() {

	transform.position.x = 272.0f;

	ne::set_font(&fonts.message);

	sent_by_label.render(data.sent_by);

	char time_formatted[64];
	time_t seconds = (time_t)data.timestamp;
	tm time_data;
	localtime_s(&time_data, &seconds);
	strftime(time_formatted, 64, "%H:%I", &time_data);

	time_label.render(STRING("at " << time_formatted));

	float x_start = transform.position.x + 16.0f;

	ne::vector3f position = {
		x_start,
		0.0f,
		0.0f
	};

	// Find all instances of emojis.
	std::vector<std::string> emoji_types = { ":)", ":(", ":D", ":P", "8}", ":drunk:", ":giggle:" };
	std::vector<message_emoji_index> emoji_indices;
	size_t ii = 0;
	for (auto& i : emoji_types) {
		size_t emoji_position = data.body.find(i);
		while (emoji_position != std::string::npos) {
			message_emoji_index emoji;
			emoji.emoji_index = ii;
			emoji.position = emoji_position;
			emoji_position = data.body.find(i, emoji_position + 1);
			emoji_indices.push_back(emoji);
		}
		ii++;
	}

	// Sort the indices
	std::sort(emoji_indices.begin(), emoji_indices.end(), [](const message_emoji_index& a, const message_emoji_index& b) -> bool {
		return a.position < b.position;
	});

	// Split text into parts
	size_t str_pos = 0;
	body_height = 0.0f;
	if (emoji_indices.size() == 0) {
		if (data.body != "" && data.body != "\n") {
			chat_message_part part;
			part.label.transform.position = position;
			part.label.render(data.body);
			body.emplace_back(part);
			body_height += part.label.transform.scale.height;
		}
		str_pos = data.body.size();
	} else {
		body_height = 25.0f;
	}

	for (auto& i : emoji_indices) {

		size_t newlines = 0;
		float line_height = 25.0f;

		// Render the text part
		chat_message_part part;
		part.label.text = data.body.substr(str_pos, i.position - str_pos);
		bool is_x_translated = true;
		if (part.label.text != "") {
			if (part.label.text != "\n") {
				for (size_t j = 0; j < part.label.text.size(); j++) {
					if (part.label.text[j] == '\n') {
						newlines++;
						if (j == 0) {
							position.x = x_start;
						} else {
							is_x_translated = false;
						}
					}
				}
				part.label.render();
				part.label.transform.position = position;
				body.emplace_back(part);
				line_height = part.label.transform.scale.height;
			} else {
				newlines++;
				position.x = x_start;
			}
		}

		// Update string position
		str_pos = i.position + emoji_types[i.emoji_index].size();

		// Update transform
		if (newlines != 0) {
			position.y += line_height;
			body_height += line_height;
		}
		if (is_x_translated) {
			position.x += part.label.transform.scale.width;
		} else {
			position.x = x_start;
		}

		// Render the emoji that we found
		chat_emoji emoji;
		emoji.transform.position = position;
		emoji.transform.position.y -= 6.0f;
		emoji.transform.scale.xy = { 25.0f, 25.0f };
		emoji.identifier = emoji_types[i.emoji_index];
		emoji.texture = &textures.emojis[i.emoji_index];
		emojis.push_back(emoji);

		// Update transform for next text
		position.x += emoji.transform.scale.width;

	}

	// Render the last text part
	if (str_pos < data.body.size()) {
		chat_message_part part;
		part.label.transform.position = position;
		part.label.text = data.body.substr(str_pos);
		if (part.label.text != "" && part.label.text != "\n") {
			if (part.label.text[0] == '\n') {
				part.label.transform.position.x = x_start;
			}
			part.label.render();
			body.emplace_back(part);
			body_height += part.label.transform.scale.height;
		}
	}

	// Now that we have all the text parts, we can check each one if they contain links.
	std::vector<std::string> link_prefixes = { "http://", "https://" };
	std::vector<char> link_breakers = { ' ', '\n', '\r', '\t', '"', '\'' };

	for (auto& prefix : link_prefixes) {

		for (size_t i = 0; i < body.size(); i++) {

			std::string text = body[i].label.get_rendered_text();

			size_t link_position = text.find(prefix);

			while (link_position != std::string::npos) {

				// Check if link is broken
				size_t next_space = text.size();
				for (auto& breaker : link_breakers) {
					size_t breaker_position = text.find(breaker, link_position);
					if (breaker_position != std::string::npos) {
						next_space = breaker_position;
						break;
					}
				}

				// Is the entire part the url?
				if (link_position == 0 && next_space == text.size() - 1) {

					// Yes.
					body[i].is_link = true;

					break;
				}

				chat_message_part before;
				chat_message_part after;
				bool add_before = false;
				bool add_after = false;

				// Is there text before the URL?
				if (link_position > 0) {

					// Yes.
					add_before = true;
					before.label.transform.position = body[i].label.transform.position;
					before.label.text = text.substr(0, link_position);
					before.label.render();

					body[i].label.transform.position.x += before.label.transform.scale.width;

				}

				body[i].is_link = true;
				body[i].label.text = text.substr(link_position, next_space - link_position);
				body[i].label.render();

				// Is there text after the URL?
				if (next_space < text.size()) {

					// Yes.
					add_after = true;
					after.label.text = text.substr(next_space);
					after.label.transform.position = body[i].label.transform.position;
					after.label.render();

					after.label.transform.position.x += body[i].label.transform.scale.width;

				}

				if (add_before) {
					body.emplace(body.begin() + i, before);
					i++;
				}

				if (add_after) {
					body.emplace(body.begin() + i, after);
					i++;
				}

				link_position = text.find(prefix, link_position + 1);

			}

		}


	}

}

void chat_message::update() {

	transform.scale.width = ne::get_ortho_camera()->width() - 272.0f * 2.0f;
	transform.scale.height = body_height + 8.0f;

	if (!hide_header) {
		transform.scale.height += sent_by_label.transform.scale.height + 16.0f;
	}

	if (show_separator) {
		transform.scale.height += 16.0f;
	}

}

void chat_message::draw() {

	// Draw the sent line
	if (!hide_header) {

		ne::set_color(1.0f, 1.0f, 1.0f, 1.0f);
		sent_by_label.transform.position.x = transform.position.x + 8.0f;
		sent_by_label.transform.position.y = transform.position.y + 8.0f;
		sent_by_label.draw();

		ne::set_color(0.5f, 0.5f, 0.5f, 0.6f);
		time_label.transform.position = sent_by_label.transform.position;
		time_label.transform.position.x += sent_by_label.transform.scale.width + 16.0f;
		time_label.draw();

	}

	// Draw the text parts
	for (auto& i : body) {

		if (i.is_link) {
			ne::set_color(0.5f, 0.6f, 1.0f, 1.0f);
		} else {
			ne::set_color(1.0f, 1.0f, 1.0f, 0.9f);
		}

		if (!hide_header) {
			i.label.transform.position.y += sent_by_label.transform.scale.height + 16.0f;
		}

		i.label.transform.position.y += transform.position.y;

		i.label.draw();

		i.label.transform.position.y -= transform.position.y;

		if (!hide_header) {
			i.label.transform.position.y -= sent_by_label.transform.scale.height + 16.0f;
		}
	}

	// Draw emoji parts
	ne::set_color(1.0f, 1.0f, 1.0f, 1.0f);
	for (auto& i : emojis) {

		i.transform.position.y += transform.position.y;

		if (!hide_header) {
			i.transform.position.y += sent_by_label.transform.scale.height + 16.0f;
		}

		ne::set_model_matrix(&i.transform);
		ne::set_texture(i.texture);
		ne::draw_vertices();

		i.transform.position.y -= transform.position.y;

		if (!hide_header) {
			i.transform.position.y -= sent_by_label.transform.scale.height + 16.0f;
		}
	}

	// Draw separator
	if (show_separator) {
		
		ne::transform3f sep;
		sep.position = transform.position;
		sep.position.y += transform.scale.height - 8.0f;
		sep.scale.xy = { transform.scale.width, 2.0f };

		ne::set_color(62.0f / 255.0f, 65.0f / 255.0f, 70.0f / 255.0f, 1.0f);
		ne::set_model_matrix(&sep);
		ne::set_texture(&textures.blank);
		ne::draw_vertices();

	}

}

void chat_image::render() {

	transform.position.x = 272.0f;

	std::string path = STRING(data.timestamp << ".a4.dat");

	ne::write_file(path, data.image.begin, data.image.size());

	image.path = path;
	image.load_file();
	image.render();

}

void chat_image::update() {

	transform.scale.width = ne::get_ortho_camera()->width() - 272.0f * 2.0f;
	transform.scale.height = (float) image.size.height + 8.0f;
	transform.scale.height += sent_by_label.transform.scale.height + 16.0f;
	transform.scale.height += 16.0f;

}

void chat_image::draw() {

	// Draw the sent line
	ne::set_color(1.0f, 1.0f, 1.0f, 1.0f);
	sent_by_label.transform.position.x = transform.position.x + 8.0f;
	sent_by_label.transform.position.y = transform.position.y + 8.0f;
	sent_by_label.draw();

	ne::set_color(0.5f, 0.5f, 0.5f, 0.6f);
	time_label.transform.position = sent_by_label.transform.position;
	time_label.transform.position.x += sent_by_label.transform.scale.width + 16.0f;
	time_label.draw();

	// Draw image
	ne::transform3f image_transform;
	image_transform.position = transform.position;
	image_transform.scale.xy = {
		(float)image.size.width,
		(float)image.size.height
	};
	ne::set_color(1.0f, 1.0f, 1.0f, 1.0f);
	ne::set_texture(&image);
	ne::set_model_matrix(&image_transform);
	ne::draw_vertices();

}