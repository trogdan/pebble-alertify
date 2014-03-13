#include "pebble.h"

static Window *g_root_window;

// This is a scroll layer
//static ScrollLayer *scroll_layer;

// We also use a text layer to scroll in the scroll layer / splash screen
static TextLayer *g_primary_text_layer;
static char g_splash_text[] = "Alertify";

// The scroll layer can other things in it such as an invert layer
//static InverterLayer *inverter_layer;

// Lorum ipsum to have something to scroll
//static char scroll_text[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam quam tellus, fermentu  m quis vulputate quis, vestibulum interdum sapien. Vestibulum lobortis pellentesque pretium. Quisque ultricies purus e  u orci convallis lacinia. Cras a urna mi. Donec convallis ante id dui dapibus nec ullamcorper erat egestas. Aenean a m  auris a sapien commodo lacinia. Sed posuere mi vel risus congue ornare. Curabitur leo nisi, euismod ut pellentesque se  d, suscipit sit amet lorem. Aliquam eget sem vitae sem aliquam ornare. In sem sapien, imperdiet eget pharetra a, lacin  ia ac justo. Suspendisse at ante nec felis facilisis eleifend.";

//static const int vert_scroll_text_padding = 4;

//root window doubles as a splash screen, shown while waiting for alert from phone
static void root_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	GRect max_text_bounds = GRect(0, 40, bounds.size.w, bounds.size.h);
	
	// Initialize the text layer
	g_primary_text_layer = text_layer_create(max_text_bounds);
	text_layer_set_text(g_primary_text_layer, g_splash_text);
	
	// Change the font to a nice readable one
	text_layer_set_font(g_primary_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
	text_layer_set_text_alignment(g_primary_text_layer, GTextAlignmentCenter);
	text_layer_set_background_color(g_primary_text_layer, GColorClear);
	  
	// Trim text layer and scroll content to fit text box
	//GSize max_size = text_layer_get_content_size(g_primary_text_layer);
	//text_layer_set_size(g_primary_text_layer, max_size);
	
	// The inverter layer will highlight some text
	//inverter_layer = inverter_layer_create(GRect(0, 28, bounds.size.w, 28));
	//scroll_layer_add_child(scroll_layer, inverter_layer_get_layer(inverter_layer));
	
	layer_add_child(window_layer, text_layer_get_layer(g_primary_text_layer));
}

static void root_window_appear(Window *window) {

}

static void root_window_disappear(Window *window) {
	
}

static void root_window_unload(Window *window) {
	text_layer_destroy(g_primary_text_layer);
}

int main(void) {
	g_root_window = window_create();
	window_set_window_handlers(g_root_window, (WindowHandlers) {
		.load = root_window_load,
		.appear = root_window_appear,
		.disappear = root_window_disappear,
		.unload = root_window_unload,
	});
	window_stack_push(g_root_window, true /* Animated */);
	
	app_event_loop();
	
	window_destroy(g_root_window);
}
