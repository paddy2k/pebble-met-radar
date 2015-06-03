// #include <pebble.h>
// #include <math.h>

// void getMetTime(int counter, struct tm *tmA){
//   struct tm *tm;
//   time_t then;
  
//   char *baseURL = "http://smallroomstudios.net/met-pebble/index.php?t=%d%02d%02d%02d%02d";
  
//   int one_mins = 60;
//   int fifteen_mins = 15 * one_mins;
  
//   then = time(NULL);
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "A: %d", (int) then);
  
//   // Allow for the source image to be uploaded first
//   then += one_mins * 7;
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "B: %d", (int) then);
  
//   // Update 
//   then -= fifteen_mins * counter;
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "C: %d", (int) then);
  
//   tm = localtime(&then);
//   tm->tm_min = floor(tm->tm_min/15)*15; // Round to nearest 15min
//   tm->tm_mon++; // Set correct month
//   tm->tm_year += 1900; // Set correct year
    
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "*** DEBUG STARTED ***");
  
//   APP_LOG(APP_LOG_LEVEL_DEBUG, baseURL, tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min);
  
//   *tmA = tm;
// }