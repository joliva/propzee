#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;


static void main_window_load(Window *window) {
	// Create time TextLayer
	s_time_layer = text_layer_create(GRect(0, 55, 144, 50));
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorBlack);
	text_layer_set_text(s_time_layer, "00:00");

	// Improve the layout to be more like a watchface
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	// Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
	// Destroy TextLayer
	text_layer_destroy(s_time_layer);
}

static void update_time() {
	// Get a tm structure
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);

	// Create a long-lived buffer
	static char buffer[] = "00:00";

	// Write the current hours and minutes into the buffer
	if(clock_is_24h_style() == true) {
	  // Use 24 hour format
	  strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	} else {
	  // Use 12 hour format
	  strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
	}

	// Display this time on the TextLayer
	text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

static void send_int(int key, int value) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_int(iter, key, &value, sizeof(int), true);
  app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "select button pressed");

	send_int(1,1);

/*
	// Prepare dictionary
	DictionaryIterator *iterator;
	app_message_outbox_begin(&iterator);

	// Write data
	int key = 78;
	int value = 2113;
	dict_write_int(iterator, key, &value, sizeof(int), true );

	// Send the data!
	app_message_outbox_send();
*/
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  //window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  //window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void out_sent_handler(DictionaryIterator *sent, void *context){}
static void out_fail_handler(DictionaryIterator *failed, AppMessageResult reason, void* context){}
static void in_received_handler(DictionaryIterator *iter, void* context){}
void in_drop_handler(AppMessageResult reason, void *context){}

static void init() {
	// Create main Window element and assign to pointer
	s_main_window = window_create();

	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
	  .load = main_window_load,
	  .unload = main_window_unload
	});

	// Show the Window on the watch, with animated=true
	window_stack_push(s_main_window, true);

	// Register the ClickHandlers
	window_set_click_config_provider(s_main_window, click_config_provider);

	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	// Register message handlers
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_drop_handler);
	app_message_register_outbox_failed(out_fail_handler);

	// Init buffers
	app_message_open(64, 64);
}

static void deinit() {
	// Destroy Window
	window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

