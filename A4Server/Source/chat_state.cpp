#include "chat_state.hpp"
#include "assets.hpp"

void chat_state::add_client_listeners(size_t index) {

	sockets[index].listen([this, index](ne::io_socket::disconnect_message disconnect) {

		// Do we have this user registered?
		if (!users.contains(clients[index].user.name)) {
			return;
		}

		// Tell the other clients this user went offline.
		user_info_packet user_info;
		user_info.data.name = clients[index].user.name;
		user_info.data.status = A4_USER_STATUS_OFFLINE;
		sockets.broadcast(user_info);

		// Update server state.
		users.update_user_info(user_info.data);

		// Reset this client.
		clients[index] = {};

	});

	sockets[index].listen([this, index](ne::io_socket::receive_packet_message receive) {

		uint16 type = receive.packet.buffer.read_uint16();

		DEBUG(0, NE_INFO, "Got packet " << type);

		switch (type) {

		case A4_PACKET_MESSAGE: {

			// Read packet.
			message_packet packet;
			packet.parse(&receive.packet);

			// Add message to state.
			channels[packet.data.channel].data.messages.push_back(packet.data);
			channels[packet.data.channel].data.order.push_back(A4_PACKET_MESSAGE);

			// Broadcast the message.
			sockets.broadcast(packet, index);

			break;
		}

		case A4_PACKET_IMAGE: {

			// Read packet.
			image_packet packet;
			packet.parse(&receive.packet);

			// Add message to state.
			channels[packet.data.channel].data.images.push_back(packet.data);
			channels[packet.data.channel].data.order.push_back(A4_PACKET_IMAGE);

			DEBUG(0, NE_INFO, "Received image of " << packet.data.image.size() << " bytes");

			// Broadcast the message.
			sockets.broadcast(packet, index);

			break;
		}

		case A4_PACKET_USER_INFO: {

			// Read packet.
			user_info_packet packet;
			packet.parse(&receive.packet);

			// Add user to state.
			clients[index].user = packet.data;
			users.update_user_info(packet.data);

			// Broadcast the new user info.
			sockets.broadcast(packet, index);

			break;
		}

		default:
			break;

		}

	});
}

chat_state::chat_state() {
	
	// Set our camera.
	ne::set_camera_reference(0, &camera);
	ne::set_camera(0);

	// Add channels
	channels["general"] = { "general" };
	channels["networking"] = { "networking" };
	channels["gamedev"] = { "gamedev" };
	channels["c++"] = { "c++" };
	channels["music"] = { "music" };

	// Start accepting clients
	establisher.container = &sockets;
	establisher.listen("10.0.0.130", 5950);

	// Handler for established connections.
	establisher.listen([this](ne::connection_establisher::established_message established) {

		clients[established.index] = {};

		if (sockets[established.index].begin_async()) {

			server_state_packet server_state;
			for (auto& i : channels) {
				server_state.data.channels.push_back(i.second.data);
			}
			server_state.data.users = users.users;
			server_state.finish();
			sockets[established.index].send(&server_state);

			add_client_listeners(established.index);

			sockets[established.index].end_async();

		}

	});

	fps.font = &fonts.message;

}

chat_state::~chat_state() {

}

void chat_state::update() {

	establisher.synchronise();
	sockets.synchronise();

	camera.update();

	int connected = (int) sockets.sockets.size() - (int) sockets.available_sockets.size();

	fps.render(STRING(
		"Delta " << ne::delta() << "\n"
		<< "FPS: " << ne::current_fps() << "\n"
		<< "Connections: " << connected << " / " << sockets.sockets.size()
	));

	fps.transform.position.xy = {
		camera.x() + camera.width() / 2.0f - fps.transform.scale.width / 2.0f,
		camera.y() + camera.height() / 2.0f - fps.transform.scale.height / 2.0f
	};

	textures.update();
}

void chat_state::draw() {

	camera.transform.scale.xy = ne::window_size_f();

	ne::set_drawing_shape(0);
	ne::set_shader(&shaders.basic);
	ne::set_texture(&textures.blank);
	ne::push_color(1.0f, 1.0f, 1.0f, 1.0f);

	fps.draw();

	ne::pop_color();

}
