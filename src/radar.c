#include "radar.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_radar_1;
static GBitmap *s_res_previous_button;
static BitmapLayer *s_bitmaplayer_1;
static ActionBarLayer *s_actionbarlayer_1;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorClear);
  window_set_fullscreen(s_window, 1);
  
  //s_res_radar_1 = gbitmap_create_with_resource(RESOURCE_ID_RADAR_1);
  s_res_radar_1 = gbitmap_create_with_resource(RESOURCE_ID_IMAGICK_TEST);
  
  s_res_previous_button = gbitmap_create_with_resource(RESOURCE_ID_PREVIOUS_BUTTON);
  // s_bitmaplayer_1
  s_bitmaplayer_1 = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_bitmaplayer_1, s_res_radar_1);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);
  
  // s_actionbarlayer_1
  s_actionbarlayer_1 = action_bar_layer_create();
  action_bar_layer_add_to_window(s_actionbarlayer_1, s_window);
  action_bar_layer_set_background_color(s_actionbarlayer_1, GColorBlack);
  action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_DOWN, s_res_previous_button);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_actionbarlayer_1);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_bitmaplayer_1);
  action_bar_layer_destroy(s_actionbarlayer_1);
  gbitmap_destroy(s_res_radar_1);
  gbitmap_destroy(s_res_previous_button);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_radar(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_radar(void) {
  window_stack_remove(s_window, true);
}
