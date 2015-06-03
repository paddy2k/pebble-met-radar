#include <pebble.h>
#include "netdownload.h"
#include "radar.h"
#include "loading.h"
#include <math.h>

int main(void) {
  show_loading();
  net_init();

  app_event_loop();

  hide_radar();
  netdownload_deinitialize();
}