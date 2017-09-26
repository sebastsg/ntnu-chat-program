#include "channel.hpp"
#include "assets.hpp"

void chat_channel::update() {
	history.update();
}

void chat_channel::draw() {
	history.draw();
}

void channel_list::update() {

	transform.position.x = 0.0f;
	transform.position.y = 4.0f;
	transform.scale.width = 252.0f;
	transform.scale.height = ne::get_ortho_camera()->height() - 8.0f;

	float y = transform.position.y + 8.0f;

	for (auto& i : channels) {

		// Update label
		i.second.button.transform.position.x = transform.position.x + 4.0f;
		i.second.button.transform.position.y = y;
		i.second.button.transform.scale.xy = { transform.scale.width - 8.0f, 48.0f };
		i.second.button.update();

		// Prepare for next row
		y += i.second.button.transform.scale.height + 4.0f;

	}

	if (channels.find(active_channel) != channels.end()) {
		channels[active_channel].update();
	}

}

void channel_list::draw() {

	// Draw current channel history.
	if (channels.find(active_channel) != channels.end()) {
		channels[active_channel].draw();
	}

	// Draw background.
	ne::set_color(47.0f / 255.0f, 49.0f / 255.0f, 54.0f / 255.0f, 1.0f);
	ne::set_model_matrix(&transform);
	ne::set_texture(&textures.blank);
	ne::draw_vertices();

	// Draw channel names
	ne::set_color(1.0f, 1.0f, 1.0f, 1.0f);
	for (auto& i : channels) {
		i.second.button.sprite = &textures.button;
		if (i.first == active_channel) {
			// tiny hack to make button always show in pressed state
			i.second.button.transition.next_frame = 2.0f;
			i.second.button.transition.current = 1.0f;
		}
		i.second.button.draw();
	}

}

void channel_list::add_message(const message_packet::fields& data) {
	channels[data.channel].history.add_message(data);
}

void channel_list::add_image(const image_packet::fields& data) {
	channels[data.channel].history.add_image(data);
}

void channel_list::add_channel(const channel_packet::fields& data) {

	if (channels.find(data.name) == channels.end()) {
		channels[data.name] = {};
		channels[data.name].name = data.name;
		channels[data.name].button.label.render("#" + data.name);
		channels[data.name].button.label_align = ne::align_type::left;
		channels[data.name].button.label_padding = { 16.0f, 0.0f };
		std::string channel_name = data.name;
		channels[data.name].button.click.listen([this, channel_name] {
			load_channel(channel_name);
		});
	}
	
	size_t message_index = 0;
	size_t image_index = 0;

	for (auto& i : data.order) {
		
		switch (i) {

		case A4_PACKET_MESSAGE:
			add_message(data.messages[message_index]);
			message_index++;
			break;

		case A4_PACKET_IMAGE:
			add_image(data.images[image_index]);
			image_index++;
			break;
			
		default:
			break;

		}

	}

}

void channel_list::load_channel(const std::string& name) {
	if (channels.find(name) == channels.end()) {
		return;
	}
	active_channel = name;
}
