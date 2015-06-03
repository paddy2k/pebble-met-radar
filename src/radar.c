#include <pebble.h>
#include "netdownload.h"
#include "radar.h"
#include "loading.h"
#include <math.h>
  
static Window *window;
static BitmapLayer *bitmap_layer;
static GBitmap *current_bmp;
static GFont s_res_gothic_24_bold;
static TextLayer *s_textlayer_1;

static unsigned long image = 0;
static bool playing = false;
static int max_images = 24; // Every 15 mins for 6 hours
char time_stamp[6] = "00:00";

void get_image(int counter) {
  struct tm *tm;
  time_t then;
  
  char *baseURL = "http://smallroomstudios.net/met-pebble/index.php?t=%d%02d%02d%02d%02d";
  char finalURL[64];
  
  int one_mins = 60;
  int fifteen_mins = 15 * one_mins;
  image = counter;
  
  then = time(NULL);
  then += one_mins * 7;  // Allow 7mins for the source image to be uploaded first
  then -= fifteen_mins * (counter+1); // Decrement by n 15 minute blocks
  
  tm = localtime(&then);
  tm->tm_min = floor(tm->tm_min/15)*15; // Round to nearest 15min
  tm->tm_mon++; // Set correct month
  tm->tm_year += 1900; // Set correct year
    
  APP_LOG(APP_LOG_LEVEL_DEBUG, "*** GET IMAGE ***");
  APP_LOG(APP_LOG_LEVEL_DEBUG, baseURL, tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min);
  
  snprintf(finalURL, sizeof(finalURL), baseURL, tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min);
  
  snprintf(time_stamp, sizeof(time_stamp), "%02d:%02d", tm->tm_hour, tm->tm_min);
  netdownload_request(finalURL);
}


void show_next_image() {
  image++;
  if ((int) image > max_images) {
    image = 0;
  }
 
  get_image(image);
}

void show_prev_image() {  
  image--;
  if ((int) image < 0) {
    image = max_images;
  }

  get_image(image);
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  bitmap_layer = bitmap_layer_create(bounds);
  layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
  bitmap_layer_set_bitmap(bitmap_layer, current_bmp);
  
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD); 
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(4, -3, 100, 24));
  text_layer_set_background_color(s_textlayer_1, GColorClear);
  text_layer_set_text_color(s_textlayer_1, GColorWhite);
  text_layer_set_text(s_textlayer_1, time_stamp);
  text_layer_set_font(s_textlayer_1, s_res_gothic_24_bold);
  layer_add_child(window_layer, (Layer *)s_textlayer_1);
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  
  if(playing){
    playing = false;
//     action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_SELECT, s_res_play_button);
  }
  else{
    playing = true;
//     action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_SELECT, s_res_pause_button);
  }
  show_prev_image();
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  if(playing){
    show_prev_image(); 
  }
}

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_next_image();
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_prev_image();
}

void click_config_provider(void *context)
{
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void tap_handler(AccelAxisType axis, int32_t direction)
{
  get_image(0);
}

void hide_radar(void) {
  netdownload_deinitialize(); // call this to avoid 20B memory leak
  tick_timer_service_unsubscribe();
  text_layer_destroy(s_textlayer_1);
  accel_tap_service_unsubscribe();
  window_destroy(window);
}

static void window_unload(Window *window) {
  bitmap_layer_destroy(bitmap_layer);
  gbitmap_destroy(current_bmp);
}


void show_radar(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  window_set_click_config_provider(window, click_config_provider); 
  
  tick_timer_service_subscribe(SECOND_UNIT, (TickHandler) tick_handler);
  accel_tap_service_subscribe(tap_handler);
  
  hide_loading();
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
  free(download->data);
  
  // We null it out now to avoid a double free
  download->data = NULL;
  netdownload_destroy(download);  
  
  if(!window){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "No Window Exists");
    show_radar();
  }
}

void net_init(void) {
  // Need to initialize this first to make sure it is there when
  // the window_load function is called by window_stack_push.
  netdownload_initialize(download_complete_handler);
}
  