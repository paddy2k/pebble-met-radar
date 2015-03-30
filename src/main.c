#include <pebble.h>
#include "netdownload.h"
#include "radar.h"
  
static Window *window;
static TextLayer *text_layer;
static BitmapLayer *bitmap_layer;
static GBitmap *current_bmp;
static ActionBarLayer *s_actionbarlayer_1;
static GBitmap *s_res_prev_button;
static GBitmap *s_res_next_button;
static GBitmap *s_res_play_button;
static GBitmap *s_res_pause_button;

static char *images[] = {
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301200",
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301215",
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301230",
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301245",
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301300",
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301315",
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301330",
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301345",
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301400",
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301415",
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301430",
  "http://smallroomstudios.net/met-pebble/index.php?t=201503301445"
};

static unsigned long image = 0;
static bool playing = false;
static bool running = false;

void show_next_image() {
  // show that we are loading by showing no image
//   bitmap_layer_set_bitmap(bitmap_layer, NULL);

//   text_layer_set_text(text_layer, "Loading...");

  // Unload the current image if we had one and save a pointer to this one
//   if (current_bmp) {
//     gbitmap_destroy(current_bmp);
//     current_bmp = NULL;
//   }

  if (image >= sizeof(images)/sizeof(char*)) {
    image = 0;
  }
  image++;
  
  netdownload_request(images[image]);
}

void show_prev_image() {
  // show that we are loading by showing no image
//   bitmap_layer_set_bitmap(bitmap_layer, NULL);

//   text_layer_set_text(text_layer, "Loading...");

  // Unload the current image if we had one and save a pointer to this one
//   if (current_bmp) {
//     gbitmap_destroy(current_bmp);
//     current_bmp = NULL;
//   }
  
  
  if (image == 0) {
    image = sizeof(images)/sizeof(char*);
  }
  image--;

  netdownload_request(images[image]);
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_res_prev_button = gbitmap_create_with_resource(RESOURCE_ID_PREV_BUTTON);
  s_res_next_button = gbitmap_create_with_resource(RESOURCE_ID_NEXT_BUTTON);
  s_res_play_button = gbitmap_create_with_resource(RESOURCE_ID_PLAY_BUTTON);
  s_res_pause_button = gbitmap_create_with_resource(RESOURCE_ID_PAUSE_BUTTON);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Shake it!");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  bitmap_layer = bitmap_layer_create(bounds);
  layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
  current_bmp = NULL;
  
  s_actionbarlayer_1 = action_bar_layer_create();
  action_bar_layer_add_to_window(s_actionbarlayer_1, window);
  action_bar_layer_set_background_color(s_actionbarlayer_1, GColorWhite);
  action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_UP, s_res_prev_button);
  action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_SELECT, s_res_play_button);
  action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_DOWN, s_res_next_button);
  layer_add_child(window_layer, (Layer *)s_actionbarlayer_1);
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  bitmap_layer_destroy(bitmap_layer);
  gbitmap_destroy(current_bmp);
  
  action_bar_layer_destroy(s_actionbarlayer_1);
  gbitmap_destroy(s_res_prev_button);
  gbitmap_destroy(s_res_next_button);
  gbitmap_destroy(s_res_play_button);
  gbitmap_destroy(s_res_pause_button);
}

void download_complete_handler(NetDownload *download) {
  printf("Loaded image with %lu bytes", download->length);
  printf("Heap free is %u bytes", heap_bytes_free());
 
  if (current_bmp) {
    gbitmap_destroy(current_bmp);
    current_bmp = NULL;
  }
  
  bitmap_layer_set_bitmap(bitmap_layer, NULL);
  
  #ifdef PBL_PLATFORM_APLITE
  GBitmap *bmp = gbitmap_create_with_png_data(download->data, download->length);
  #else
    GBitmap *bmp = gbitmap_create_from_png_data(download->data, download->length);
  #endif
  bitmap_layer_set_bitmap(bitmap_layer, bmp);

  // Save pointer to currently shown bitmap (to free it)
  if (current_bmp) {
    gbitmap_destroy(current_bmp);
  }
  current_bmp = bmp;

  // Free the memory now
  #ifdef PBL_PLATFORM_APLITE
  // gbitmap_create_with_png_data will free download->data
  #else
    free(download->data);
  #endif
  // We null it out now to avoid a double free
  download->data = NULL;
  netdownload_destroy(download);
  
  running = false;
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  
  if(playing){
    playing = false;
    action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_SELECT, s_res_play_button);
  }
  else{
    playing = true;
    action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_SELECT, s_res_pause_button);
  }
  show_next_image();
}

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_prev_image();
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_next_image();
}

void click_config_provider(void *context)
{
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  if(playing && !running){
    show_next_image(); 
    running = true;
  }
}

static void init(void) {
  // Need to initialize this first to make sure it is there when
  // the window_load function is called by window_stack_push.
  netdownload_initialize(download_complete_handler);

  window = window_create();
  window_set_fullscreen(window, true);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  window_set_click_config_provider(window, click_config_provider);

  //accel_tap_service_subscribe(tap_handler);
  tick_timer_service_subscribe(SECOND_UNIT, (TickHandler) tick_handler);
}

static void deinit(void) {
  netdownload_deinitialize(); // call this to avoid 20B memory leak
  window_destroy(window);
}
  
int main(void) {
  //show_radar();
  init();
  app_event_loop();
  //hide_radar();
  deinit();
}
