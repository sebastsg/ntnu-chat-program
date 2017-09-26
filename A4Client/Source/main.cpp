#include "GLEW/glew.h"
#include "SDL/SDL.h"
#include "window.hpp"
#include "assets.hpp"
#include "graphics.hpp"
#include "login_state.hpp"

void start() {

	ne::create_quad();
	ne::create_quad();

	ne::maximise_window();
	ne::set_swap_interval(NE_SWAP_INTERVAL_SYNC);

	create_asset_container();

	ne::start_network();

	ne::swap_state<login_state>();

}

void stop() {
	destroy_asset_container();
	ne::stop_network();
}

int main(int argc, char** argv) {
	ne::start_engine("A4 Client", 1200, 800);
	return ne::enter_loop(start, stop);
}