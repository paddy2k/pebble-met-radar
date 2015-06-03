#include "loading.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_met_logo_3;
static GFont s_res_roboto_condensed_21;
static BitmapLayer *s_logo_layer;
static TextLayer *s_textlayer_1;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorTiffanyBlue);
  
  s_res_met_logo_3 = gbitmap_create_with_resource(RESOURCE_ID_MET_LOGO_3);
  s_res_roboto_condensed_21 = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  // s_logo_layer
  s_logo_layer = bitmap_layer_create(GRect(37, 36, 70, 122));
  bitmap_layer_set_bitmap(s_logo_layer, s_res_met_logo_3);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_logo_layer);
  
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(0, 8, 144, 49));
  text_layer_set_background_color(s_textlayer_1, GColorClear);
  text_layer_set_text_color(s_textlayer_1, GColorWhite);
  text_layer_set_text(s_textlayer_1, "Powered by");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_1, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_logo_layer);
  text_layer_destroy(s_textlayer_1);
  gbitmap_destroy(s_res_met_logo_3);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_loading(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_loading(void) {
  window_stack_remove(s_window, true);
}
