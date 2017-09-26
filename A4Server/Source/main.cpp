#include "GLEW/glew.h"
#include "SDL/SDL.h"
#include "window.hpp"
#include "assets.hpp"
#include "graphics.hpp"
#include "chat_state.hpp"

void start() {

	ne::create_quad();
	ne::create_quad();

	ne::set_swap_interval(NE_SWAP_INTERVAL_IMMEDIATE);

	create_asset_container();

	ne::start_network();

	ne::swap_state<chat_state>();

}

void stop() {
	destroy_asset_container();
	ne::stop_network();
}

int main(int argc, char** argv) {
	ne::start_engine("A4 Server", 320, 192);
	return ne::enter_loop(start, stop);
}