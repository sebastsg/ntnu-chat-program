#include "history.hpp"
#include "debug.hpp"

chat_history::chat_history(chat_history&& other) {
	messages = other.messages;
	other.messages.clear();
}

chat_history& chat_history::operator=(chat_history&& other) {
	if (&other == this) {
		return *this;
	}
	messages = other.messages;
	other.messages.clear();
	return *this;
}

chat_history::~chat_history() {
	for (auto& i : messages) {
		delete i;
	}
}

void chat_history::add_message(const message_packet::fields& data) {
	chat_message* message = new chat_message();
	message->data = data;
	message->render();
	messages.push_back(message);
}

void chat_history::add_image(const image_packet::fields& data) {
	chat_image* image = new chat_image();
	image->data = data;
	image->render();
	messages.push_back(image);
}

void chat_history::update() {

	float y = ne::get_ortho_camera()->height() - 144.0f;

	if (messages.size() > 0) {
		y -= messages[messages.size() - 1]->transform.scale.height + 4.0f;
	}

	for (int i = (int)messages.size() - 1; i >= 0; i--) {

		messages[i]->transform.position.y = y;
		messages[i]->update();

		if (i > 0) {

			if (messages[i - 1]->sent_by() == messages[i]->sent_by()) {

				long time_difference = abs((long)messages[i - 1]->timestamp() - (long)messages[i]->timestamp());
				messages[i]->hide_header = time_difference < 30;

			}

			y -= messages[i - 1]->transform.scale.height + 4.0f;
		}

	}

	for (int i = 0; i < (int)messages.size() - 1; i++) {

		// Is the next message by someone else?
		if (messages[i + 1]->sent_by() != messages[i]->sent_by()) {

			// Yes, let's show the border on this one.
			messages[i]->show_separator = true;

			continue;

		}

		// The message is by the same person, so let's check the time difference:
		long time_difference = abs((long)messages[i + 1]->timestamp() - (long)messages[i]->timestamp());

		// If the message was over 30 seconds ago, we should show the border.
		if (time_difference >= 30) {
			messages[i]->show_separator = true;
		}

	}

}

void chat_history::draw() {
	for (auto& i : messages) {
		i->draw();
	}
}