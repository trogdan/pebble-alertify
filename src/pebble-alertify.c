#include "pebble.h"

/////////////////////////////////////////////////////////////////////////////
// Defines
/////////////////////////////////////////////////////////////////////////////

// Msg types
#define MESSAGE_TYPE_ALERT  				10

// Msg Keys
#define MESSAGE_KEY_TYPE    				1000
#define MESSAGE_KEY_TITLE   				1001
#define MESSAGE_KEY_BODY    				1002

#define PEBBLE_MAX_SCREEN_WIDTH_PIXELS		144
#define PEBBLE_MAX_SCREEN_HEIGHT_PIXELS		168

#define ALERT_MESSAGE_MAX_TEXT_HEIGHT		4096
/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
static Window *g_root_window;

// This is a scroll layer
//static ScrollLayer *scroll_layer;

// We also use a text layer to scroll in the scroll layer / splash screen
static TextLayer *g_primary_text_layer = NULL;
static ScrollLayer *g_primary_scroll_layer = NULL;
static GRect g_primary_text_bounds;

static const int g_vert_scroll_text_padding = 4;

static char g_splash_text[] = "Alertify";

// The scroll layer can other things in it such as an invert layer
//static InverterLayer *inverter_layer;

// Lorum ipsum to have something to scroll
//static char scroll_text[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam quam tellus, fermentu  m quis vulputate quis, vestibulum interdum sapien. Vestibulum lobortis pellentesque pretium. Quisque ultricies purus e  u orci convallis lacinia. Cras a urna mi. Donec convallis ante id dui dapibus nec ullamcorper erat egestas. Aenean a m  auris a sapien commodo lacinia. Sed posuere mi vel risus congue ornare. Curabitur leo nisi, euismod ut pellentesque se  d, suscipit sit amet lorem. Aliquam eget sem vitae sem aliquam ornare. In sem sapien, imperdiet eget pharetra a, lacin  ia ac justo. Suspendisse at ante nec felis facilisis eleifend.";

//static const int vert_scroll_text_padding = 4;

//root window doubles as a splash screen, shown while waiting for alert from phone
static bool g_received_alert = false;

/////////////////////////////////////////////////////////////////////////////
// Function declarations
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Function definitions
/////////////////////////////////////////////////////////////////////////////
static void root_window_load(Window *window) {
	
	//TODO make sure splash screen shows for a couple seconds if started manually
	if(g_received_alert == false)
	{
		Layer *window_layer = window_get_root_layer(window);
		GRect bounds = layer_get_frame(window_layer);
				
		// Initialize the text layer
		if(g_primary_text_layer)
		{
			layer_remove_from_parent(text_layer_get_layer(g_primary_text_layer));
			text_layer_destroy(g_primary_text_layer);
		}
		
		GRect max_text_bounds = GRect(0, 40, bounds.size.w, bounds.size.h);
		g_primary_text_layer = text_layer_create(max_text_bounds);
			
		text_layer_set_text(g_primary_text_layer, g_splash_text);
		
		// Change the font to a nice readable one
		text_layer_set_font(g_primary_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
		text_layer_set_text_alignment(g_primary_text_layer, GTextAlignmentCenter);
		text_layer_set_background_color(g_primary_text_layer, GColorClear);
		  
		layer_add_child(window_layer, text_layer_get_layer(g_primary_text_layer));
	}
}

static void window_show_message(Window *window, const char* title, const char* body, bool auto_scroll)
{
	(void)auto_scroll;
	(void)title;
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
		
	// Initialize the scroll layer
	if(g_primary_scroll_layer)
	{
		layer_remove_from_parent(scroll_layer_get_layer(g_primary_scroll_layer));
		scroll_layer_destroy(g_primary_scroll_layer);
	}
	
	g_primary_scroll_layer = scroll_layer_create(bounds);
	
	// This binds the scroll layer to the window so that up and down map to scrolling
	// You may use scroll_layer_set_callbacks to add or override interactivity
	scroll_layer_set_click_config_onto_window(g_primary_scroll_layer, window);

	// Initialize the text layer
	if(g_primary_text_layer)
	{
		layer_remove_from_parent(text_layer_get_layer(g_primary_text_layer));
		text_layer_destroy(g_primary_text_layer);
	}
	
	g_primary_text_bounds = bounds;
	g_primary_text_bounds.size.h = ALERT_MESSAGE_MAX_TEXT_HEIGHT;
	g_primary_text_layer = text_layer_create(g_primary_text_bounds);
	
	text_layer_set_text(g_primary_text_layer, body);
	
	// Change the font
	text_layer_set_font(g_primary_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(g_primary_text_layer, GTextAlignmentLeft);
	text_layer_set_background_color(g_primary_text_layer, GColorClear);
	
	//Trim as necessary to contain only the message
	g_primary_text_bounds.size = text_layer_get_content_size(g_primary_text_layer);
	text_layer_set_size(g_primary_text_layer, g_primary_text_bounds.size);
	scroll_layer_set_content_size(g_primary_scroll_layer, GSize(bounds.size.w, g_primary_text_bounds.size.h + g_vert_scroll_text_padding));

	scroll_layer_add_child(g_primary_scroll_layer, text_layer_get_layer(g_primary_text_layer));
	
	layer_add_child(window_layer, scroll_layer_get_layer(g_primary_scroll_layer));
}

static void root_window_appear(Window *window) {

}

static void root_window_disappear(Window *window) {
	/*
	// Destroy the scroll layer
	if(g_primary_text_layer)
	{
		layer_remove_from_parent(text_layer_get_layer(g_primary_text_layer));
		text_layer_destroy(g_primary_text_layer);
	}

	// Destroy the scroll layer
	if(g_primary_scroll_layer)
	{
		layer_remove_from_parent(text_layer_get_layer(g_primary_scroll_layer));
		scroll_layer_destroy(g_primary_scroll_layer);
	}
	*/
}

static void root_window_unload(Window *window) {
	text_layer_destroy(g_primary_text_layer);
}

static void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
 }


static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
 }


static void in_received_handler(DictionaryIterator *received, void *context) {
	// incoming message received
	Tuple *msg_tuple = dict_find(received, MESSAGE_KEY_TYPE);
	
	if(!msg_tuple) {
		//TODO log unknown msg
		return;
	}
	
	Tuple *body_tuple = NULL;
	Tuple *title_tuple = NULL;
	
	switch(msg_tuple->value->uint8)
	{
	case MESSAGE_TYPE_ALERT:
		g_received_alert = true;
		
		body_tuple = dict_find(received, MESSAGE_KEY_BODY);
		title_tuple = dict_find(received, MESSAGE_KEY_TITLE);
		
		window_show_message(
				g_root_window, 
				title_tuple->value->cstring, 
				body_tuple->value->cstring, 
				false);
		break;
	}
 }


static void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }

int main(void) {
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
	   
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
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
