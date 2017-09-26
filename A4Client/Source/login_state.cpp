#include "login_state.hpp"
#include "assets.hpp"
#include "chat_state.hpp"

login_state::login_state() {

	ne::set_camera_reference(A4_CAMERA_LOGIN, &camera);
	ne::set_camera(A4_CAMERA_LOGIN);

	username.allow_multiline = false;
	username.font = &fonts.login_input;

	username.submit.listen([this](chat_input_box::submit_message submit) {

		username_submitted = submit.message;

	});

	ne::set_font(&fonts.login_input);
	login_label.render("Login");

}

login_state::~login_state() {
	
}

void login_state::update() {

	// Resize camera to window size.
	camera.transform.scale.xy = ne::window_size_f();

	camera.update();

	username.transform.scale.width = ne::get_ortho_camera()->width() / 3.0f;
	username.transform.scale.height = 48.0f;
	username.transform.position.x = ne::get_ortho_camera()->width() / 2.0f - username.transform.scale.width / 2.0f;
	username.transform.position.y = ne::get_ortho_camera()->height() / 2.0f - username.transform.scale.height / 2.0f;

	username.update();

	login_label.transform.position = username.transform.position;
	login_label.transform.position.y -= login_label.transform.scale.height + 16.0f;

	// Render remaining textures
	textures.update();

	if (username_submitted != "") {
		ne::swap_state<chat_state>(username_submitted);
	}

}

void login_state::draw() {

	// Reset to default draw state.
	ne::set_drawing_shape(0);
	ne::set_shader(&shaders.basic);
	ne::set_texture(&textures.blank);
	ne::push_color(1.0f, 1.0f, 1.0f, 1.0f);

	username.draw();

	login_label.draw();

	ne::pop_color();

}
