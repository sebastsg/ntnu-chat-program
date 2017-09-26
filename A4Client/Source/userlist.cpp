#include "userlist.hpp"
#include "assets.hpp"

void chat_userlist::update() {

	transform.position.x = ne::get_ortho_camera()->width() - 256.0f;
	transform.position.y = 4.0f;
	transform.scale.width = 252.0f;
	transform.scale.height = ne::get_ortho_camera()->height() - 8.0f;

	float y = transform.position.y + 8.0f;

	ne::set_font(&fonts.message);

	for (auto& i : list.users) {
		
		// Update status icon transform
		i.status_transform.position.x = transform.position.x + 8.0f;
		i.status_transform.position.y = y;
		i.status_transform.scale.xy = { 20.0f, 20.0f };

		// Update label
		i.label.render(i.name);
		i.label.transform.position.x = transform.position.x + 40.0f;
		i.label.transform.position.y = y;

		// Prepare for next row
		y += i.label.transform.scale.height + 8.0f;

	}
}

void chat_userlist::draw() {

	// Draw background.
	ne::push_color(47.0f / 255.0f, 49.0f / 255.0f, 54.0f / 255.0f, 1.0f);
	ne::set_model_matrix(&transform);
	ne::set_texture(&textures.blank);
	ne::draw_vertices();

	// Draw usernames.
	for (auto& i : list.users) {

		switch (i.status) {

		case A4_USER_STATUS_ONLINE:
			ne::set_color(0.2f, 1.0f, 0.2f, 1.0f);
			break;

		case A4_USER_STATUS_BUSY:
			ne::set_color(1.0f, 0.2f, 0.2f, 1.0f);
			break;

		case A4_USER_STATUS_AWAY:
			ne::set_color(0.8f, 0.8f, 0.2f, 1.0f);
			break;

		case A4_USER_STATUS_OFFLINE:
		default:
			ne::set_color(1.0f, 1.0f, 1.0f, 0.5f);

		}

		ne::set_texture(&textures.blank);
		ne::set_model_matrix(&i.status_transform);
		ne::draw_vertices();

		if (i.status == A4_USER_STATUS_OFFLINE) {
			ne::set_color(1.0f, 1.0f, 1.0f, 0.5f);
		} else {
			ne::set_color(1.0f, 1.0f, 1.0f, 1.0f);
		}

		i.label.draw();

	}

	ne::pop_color();

}
