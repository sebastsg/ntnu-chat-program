#include "chat_state.hpp"
#include "assets.hpp"
#include "message.hpp"
#include "debug.hpp"
#include "platform.hpp"

chat_state::chat_state(std::string username) {
	
	ne::set_camera_reference(A4_CAMERA_CHAT, &camera);
	ne::set_camera(A4_CAMERA_CHAT);

	input_box.font = &fonts.message;
	
	send_image_button.label.font = &fonts.message;
	send_image_button.label.render("Send image");
	send_image_button.sprite = &textures.button;

	send_image_button.click.listen([this] {

		std::string path = ne::open_file_dialog();

		image_packet packet;
		packet.data.sent_by = user.name;
		packet.data.channel = channels.active_channel;
		packet.data.timestamp = (uint32)time(nullptr);
		ne::read_file(path, &packet.data.image);
		packet.finish();

		channels.add_image(packet.data);

		server.send_async(&packet);

	});

	user.name = username;
	user.status = A4_USER_STATUS_ONLINE;
	add_user(user);

	server.connect("37.200.2.197", 5950);

	user_info_packet user_packet;
	user_packet.data = user;
	user_packet.finish();
	server.send_async(&user_packet);

	server.listen([this](ne::io_socket::receive_packet_message receive) {

		uint16 type = receive.packet.buffer.read_uint16();

		switch (type) {

		case A4_PACKET_MESSAGE: {
			message_packet packet;
			packet.parse(&receive.packet);
			channels.add_message(packet.data);
			break;
		}

		case A4_PACKET_IMAGE: {
			image_packet packet;
			packet.parse(&receive.packet);
			channels.add_image(packet.data);
			break;
		}

		case A4_PACKET_CHANNEL: {
			channel_packet packet;
			packet.parse(&receive.packet);
			channels.add_channel(packet.data);
			break;
		}

		case A4_PACKET_SERVER_STATE: {
			server_state_packet packet;
			packet.parse(&receive.packet);
			for (auto& i : packet.data.channels) {
				channels.add_channel(i);
			}
			for (auto& i : packet.data.users) {
				add_user(i);
			}
			channels.load_channel("general");
			break;
		}

		case A4_PACKET_USER_INFO: {
			user_info_packet packet;
			packet.parse(&receive.packet);
			add_user(packet.data);
			break;
		}

		default:
			break;

		}

	});

	input_box.submit.listen([this](chat_input_box::submit_message submit) {

		// Prepare packet for server.
		message_packet packet;
		packet.data.sent_by = user.name;
		packet.data.channel = channels.active_channel;
		packet.data.timestamp = (uint32) time(nullptr);
		packet.data.body = submit.message;
		packet.finish();

		// Add message locally.
		channels.add_message(packet.data);

		// Send to server.
		server.send_async(&packet);

	});

}

chat_state::~chat_state() {

}

void chat_state::add_user(const user_info_packet::fields& data) {
	userlist.list.update_user_info(data);
}

void chat_state::update() {

	// Resize camera to window size.
	camera.transform.scale.xy = ne::window_size_f();

	camera.update();

	input_box.transform.position.x = 272.0f;
	input_box.transform.position.y = ne::get_ortho_camera()->height() - 128.0f;
	input_box.transform.scale.width = ne::get_ortho_camera()->width() - 272.0f * 2.0f;
	input_box.transform.scale.height = 64.0f;

	server.synchronise();

	input_box.update();
	channels.update();
	userlist.update();

	send_image_button.transform.position.xy = {
		userlist.transform.position.x + 4.0f,
		userlist.transform.position.y + userlist.transform.scale.height - 116.0f
	};

	send_image_button.transform.scale.xy = {
		userlist.transform.scale.width - 8.0f,
		48.0f
	};

	send_image_button.update();

	// Update FPS text
	//fps.render(STRING("Delta " << ne::delta() << "\nFPS: " << ne::current_fps()));

}

void chat_state::draw() {

	// Reset to default draw state.
	ne::set_drawing_shape(0);
	ne::set_shader(&shaders.basic);
	ne::set_texture(&textures.blank);
	ne::push_color(1.0f, 1.0f, 1.0f, 1.0f);

	ne::transform3f t;
	t.scale = camera.transform.scale;
	ne::set_model_matrix(&t);
	ne::push_color(54.0f / 255.0f, 57.0f / 255.0f, 62.0f / 255.0f, 1.0f);
	ne::draw_vertices();
	ne::pop_color();

	input_box.draw();
	channels.draw();
	userlist.draw();
	send_image_button.draw();

	// Draw FPS
	/*ne::set_font(&fonts.message);
	ne::set_color(1.0f, 1.0f, 1.0f, 1.0f);
	fps.transform.position.xy = camera.xy();
	fps.transform.position.x += camera.width() - 176.0f;
	fps.transform.position.y += 8.0f;
	ne::draw_text(&fps);*/

	ne::pop_color();

}
