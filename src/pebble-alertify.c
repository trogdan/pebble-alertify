#include <pebble.h>

static Window *g_wMainWindow;

// This is a menu layer
// You have more control than with a simple menu layer
static MenuLayer *g_mlMainMenu;

// Menu items can optionally have an icon drawn with them
static GBitmap *g_gbMainMenuIcons;

static uint16_t g_iCurrentMainMenuItem = 0;
static uint16_t g_iTotalMainMenuItems = 0;


static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  // Process tap on ACCEL_AXIS_X, ACCEL_AXIS_Y or ACCEL_AXIS_Z
  // Direction is 1 or -1
  // TODO Assume any tap/shake is a request to dismiss all notifications
}


static void click_handler(ClickRecognizerRef recognizer, Window *window) {
    
  switch (click_recognizer_get_button_id(recognizer)) {
	case BUTTON_ID_UP:
	  break;

	case BUTTON_ID_DOWN:
	  break;

	default:
	case BUTTON_ID_SELECT:
	  break;
  }
  

}

static void config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) click_handler);
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item we'll draw
  switch (cell_index->row) {
	case 0:
	  // This is a basic menu item with a title and subtitle
	  menu_cell_basic_draw(ctx, cell_layer, "Basic Item", "With a subtitle", NULL);
	  animate_layer(cell_layer);
	  	  
	  break;

	case 1:
	  // This is a basic menu icon with an icon
	  menu_cell_basic_draw(ctx, cell_layer, "Icon Item", "Select to cycle", g_gbMainMenuIcons[0]);
	  break;

  }

}

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  switch (cell_index->row) {
  	  break;
  }

}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Here we load the bitmap assets
  // resource_init_current_app must be called before all asset loading
  for(int16_t i=0; i < g_iTotalMainMenuItems; i++)
  {
	  g_gbMainMenuIcons[i] = gbitmap_create_with_resource(0 /*TODO some id*/);
  }
  
    // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  g_mlMainMenu = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(g_mlMainMenu, NULL, (MenuLayerCallbacks){
    .get_num_rows = menu_get_num_rows_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(g_mlMainMenu, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(g_mlMainMenu));
}

static void window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(g_mlMainMenu);

  // Cleanup the menu icons
  for (int i = 0; i < g_iTotalMainMenuItems; i++) {
    gbitmap_destroy(g_gbMainMenuIcons[i]);
  }
}


void handle_init(void) {
  accel_tap_service_subscribe(&accel_tap_handler);
}

void handle_deinit(void) {
  accel_tap_service_unsubscribe();
}

static void init(void) {
  handle_init();
  
  g_wMainWindow = window_create();
  window_set_click_config_provider(g_wMainWindow, click_config_provider);
  window_set_window_handlers(g_wMainWindow, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(g_wMainWindow, animated);
}

static void deinit(void) {
  window_destroy(g_wMainWindow);
  
  handle_deinit();
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", g_wMainWindow);

  app_event_loop();
  deinit();
}
