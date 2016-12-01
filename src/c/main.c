#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_hour_layer;
static TextLayer *s_minute_layer;
static TextLayer *s_day_layer;
static TextLayer *s_date_layer;

static BitmapLayer *s_bitmap_layer;
static GBitmap *s_bg_bitmap;

static GFont *s_time_font;
static GFont *s_date_font;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create long-lived buffers
  static char hourbuffer[] = "00";
  static char minutebuffer[] = "00";
  static char daybuffer[] = "XXXXXXXXXXXXXXXX";
  static char datebuffer[] = "XXXXXXXXXXXXXXXX";
  // Write the current hour and minute
  // See here for formatting: http://www.cplusplus.com/reference/ctime/strftime/
  strftime(hourbuffer, sizeof(hourbuffer), "%H", tick_time);
  strftime(minutebuffer, sizeof(minutebuffer), "%M", tick_time);
  strftime(daybuffer, sizeof(daybuffer), "%A", tick_time);
  strftime(datebuffer, sizeof(datebuffer), "%B %d", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_hour_layer, hourbuffer);
  text_layer_set_text(s_minute_layer, minutebuffer);
  text_layer_set_text(s_day_layer, daybuffer);
  text_layer_set_text(s_date_layer, datebuffer);
}

static void main_window_load(Window *window) {
  // Create time layers
  s_hour_layer = text_layer_create(GRect(0, 4, 144, 72));
  s_minute_layer = text_layer_create(GRect(0, 64, 144, 72));
  //Create date layers
  s_day_layer = text_layer_create(GRect(0, -4, 144, 72));
  s_date_layer = text_layer_create(GRect(0, 140, 144, 72));
  
  //Set text color and background color
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  
  text_layer_set_background_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_hour_layer, GColorGreen);
  
  text_layer_set_background_color(s_minute_layer, GColorClear);
  text_layer_set_text_color(s_minute_layer, GColorGreen);
  
  
  //Load fonts
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VERVE_72));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GROTESQUE_24));
  
  //Set Fonts
  text_layer_set_font(s_hour_layer, s_time_font);
  text_layer_set_font(s_minute_layer, s_time_font);
  text_layer_set_font(s_day_layer, s_date_font);
  text_layer_set_font(s_date_layer, s_date_font);
  
  //Set alignment
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(s_minute_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  
  //Create the GBitmap, specifying the 'Identifier' chosen earlier, prefixed with RESOURCE_ID_. This will manage the image data:
  s_bg_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG);
  
  //Declare a bitmap layer
  static BitmapLayer *s_bitmap_layer;
  
  //Create bitmap layer and set it to show the GBitmap
  s_bitmap_layer = bitmap_layer_create(GRect(0,0,144,168));
  bitmap_layer_set_bitmap(s_bitmap_layer,s_bg_bitmap);
  
  //Add the bitmaplayer as a child layer to the window so it's behind the other elements:
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bitmap_layer));
  
  // Add text layer as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minute_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
}
static void main_window_unload(Window *window) {
  // Destroy TextLayers
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_minute_layer);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_date_layer);
  
  //Destroy bitmap layer
  bitmap_layer_destroy(s_bitmap_layer);
  gbitmap_destroy(s_bg_bitmap);
  fonts_unload_custom_font(s_time_font);
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

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
  
  // Make sure the time is displayed from the start
  update_time();
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
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
