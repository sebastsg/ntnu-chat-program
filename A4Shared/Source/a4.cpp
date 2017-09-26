#include "a4.hpp"
#include "debug.hpp"

void message_packet::fields::read(ne::memory_buffer* buffer) {
	timestamp = buffer->read_uint32();
	sent_by = buffer->read_string();
	body = buffer->read_string();
	channel = buffer->read_string();
}

void message_packet::fields::write(ne::memory_buffer* buffer) {
	buffer->write_uint32(timestamp);
	buffer->write_string(sent_by);
	buffer->write_string(body);
	buffer->write_string(channel);
}

void image_packet::fields::read(ne::memory_buffer* buffer) {
	timestamp = buffer->read_uint32();
	sent_by = buffer->read_string();
	uint32 size = buffer->read_uint32();
	image.allocate(size);
	buffer->read_buffer(image.begin, size);
	channel = buffer->read_string();
}

void image_packet::fields::write(ne::memory_buffer* buffer) {
	buffer->write_uint32(timestamp);
	buffer->write_string(sent_by);
	buffer->write_uint32(image.size());
	buffer->write_buffer(image.begin, image.size());
	buffer->write_string(channel);
}

void user_info_packet::fields::read(ne::memory_buffer* buffer) {
	name = buffer->read_string();
	status = buffer->read_uint8();
}

void user_info_packet::fields::write(ne::memory_buffer* buffer) {
	buffer->write_string(name);
	buffer->write_uint8(status);
}

void channel_packet::fields::read(ne::memory_buffer* buffer) {
	name = buffer->read_string();
	size_t count = buffer->read_uint32();
	for (size_t i = 0; i < count; i++) {
		order.push_back(buffer->read_uint8());
	}
	count = buffer->read_uint32();
	for (size_t i = 0; i < count; i++) {
		message_packet::fields message;
		message.read(buffer);
		messages.push_back(message);
	}
	count = buffer->read_uint32();
	for (size_t i = 0; i < count; i++) {
		image_packet::fields image;
		image.read(buffer);
		images.push_back(image);
	}
}

void channel_packet::fields::write(ne::memory_buffer* buffer) {
	buffer->write_string(name);
	buffer->write_uint32(order.size());
	for (auto& i : order) {
		buffer->write_uint8(i);
	}
	buffer->write_uint32(messages.size());
	for (auto& i : messages) {
		i.write(buffer);
	}
	buffer->write_uint32(images.size());
	for (auto& i : images) {
		i.write(buffer);
	}
}

void server_state_packet::fields::read(ne::memory_buffer* buffer) {
	size_t count = buffer->read_uint32();
	for (size_t i = 0; i < count; i++) {
		channel_packet::fields channel;
		channel.read(buffer);
		channels.push_back(channel);
	}
	count = buffer->read_uint32();
	for (size_t i = 0; i < count; i++) {
		user_info_packet::fields user;
		user.read(buffer);
		users.push_back(user);
	}
}

void server_state_packet::fields::write(ne::memory_buffer* buffer) {
	buffer->write_uint32(channels.size());
	for (auto& i : channels) {
		i.write(buffer);
	}
	buffer->write_uint32(users.size());
	for (auto& i : users) {
		i.write(buffer);
	}
}
