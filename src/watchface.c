#include <pebble.h>
#include <time.h>
#include <stdlib.h>

#define NUM_POKEMON 15

static Window *s_main_window;
static TextLayer *s_time_layer, *s_d_label, *s_m_label, *s_w_label, *s_y_label, *s_slp_layer;
static TextLayer *s_name, *s_number, *s_level, *s_batt;
static BitmapLayer *mon_layer, *bg_layer;
static GBitmap *mon_bitmap, *bg_bitmap;
static GFont pokegb_font, pokegb_fsmall;
//static bool s_health_available;
static char *pokename[] = {"VENUSAUR", 
                          "CHARIZARD", 
                          "BLASTOISE", 
                          "PIKACHU",
                          "GENGAR",
                          "GYARADOS", 
                          "LAPRAS",
                          "EEVEE",
                          "VAPOREON",
                          "JOLTEON",
                          "FLAREON", 
                          "SNORLAX",
                          "ARTICUNO",
                          "ZAPDOS",
                          "MOLTRES"};
static char *pokenumb[] = {"003", 
                          "006", 
                          "009", 
                          "025",
                          "094",
                          "130", 
                          "131",
                          "133",
                          "134",
                          "135",
                          "136", 
                          "143",
                          "144",
                          "145",
                          "146"};
static uint32_t resources[] = {
  RESOURCE_ID_VENUSAUR,
  RESOURCE_ID_CHARIZARD,
  RESOURCE_ID_BLASTOISE,
  RESOURCE_ID_PIKACHU,
  RESOURCE_ID_GENGAR,
  RESOURCE_ID_GYARADOS,
  RESOURCE_ID_LAPRAS,
  RESOURCE_ID_EEVEE,
  RESOURCE_ID_VAPOREON,
  RESOURCE_ID_JOLTEON,
  RESOURCE_ID_FLAREON,
  RESOURCE_ID_SNORLAX,
  RESOURCE_ID_ARTICUNO,
  RESOURCE_ID_ZAPDOS,
  RESOURCE_ID_MOLTRES
  };
static int rid;

static void update_time() {
  //Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  //Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
    "%H:%M" : "%I:%M", tick_time);

  //Display time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);

  //Write day, month, week and year into buffers
  static char d_buff[6], m_buff[6], w_buff[6], y_buff[6];
  strftime(d_buff, sizeof(d_buff), "%a%d", tick_time);
  text_layer_set_text(s_d_label, d_buff);
  strftime(m_buff, sizeof(m_buff), "%b", tick_time);
  text_layer_set_text(s_m_label, m_buff);
  strftime(w_buff, sizeof(w_buff), "%W", tick_time);
  text_layer_set_text(s_w_label, w_buff);
  strftime(y_buff, sizeof(y_buff), "%Y", tick_time);
  text_layer_set_text(s_y_label, y_buff);
  text_layer_set_text(s_level, "100");
}

static void update_poke(){
  rid = rand()%NUM_POKEMON;
  if(mon_bitmap){
    gbitmap_destroy(mon_bitmap);
  }
  mon_bitmap = gbitmap_create_with_resource(resources[rid]);
  bitmap_layer_set_bitmap(mon_layer,mon_bitmap);
  text_layer_set_text(s_name, pokename[rid]);
  text_layer_set_text(s_number,pokenumb[rid]);
}

static void app_connection_handler(bool connected) {
  if(connected){
    text_layer_set_text(s_slp_layer, "");
  }
  else{
    vibes_double_pulse();
    text_layer_set_text(s_slp_layer, "SLP");
  }
}

static void kit_connection_handler(bool connected) {
   if(connected){
    text_layer_set_text(s_slp_layer, "");
  }
  else{
    vibes_double_pulse();
    text_layer_set_text(s_slp_layer, "SLP");
  }
}

static void handle_battery(BatteryChargeState charge_state) {
  //GRect chargebar;

    static char battery_text[] = "100";
  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "CHG");

  } else {
    snprintf(battery_text, sizeof(battery_text), "%d", charge_state.charge_percent);
  }
  text_layer_set_text(s_batt, battery_text);
}

/*static void health_handler(HealthEventType event, void *context) {
  main_window_update_ui();
}*/

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  int mins = tick_time->tm_min;
  update_time();
  if(mins % 10 == 0){
    update_poke();
  }
}

static void mw_load(Window *window){
  pokegb_font = fonts_load_custom_font(
    resource_get_handle(RESOURCE_ID_POKEMON_GB_20));
  pokegb_fsmall = fonts_load_custom_font(
    resource_get_handle(RESOURCE_ID_POKEMON_GB_8));
  //Obtain info on Window
  Layer *w_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(w_layer);

  //Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
    GRect(80, 96, 60, 40));
  s_d_label = text_layer_create(
    GRect(33, 84, 40, 30));
  s_m_label = text_layer_create(
    GRect(49, 100, 40, 30));
  s_w_label = text_layer_create(
    GRect(40, 116, 40, 30));
  s_y_label = text_layer_create(
    GRect(40, 132, 40, 30));

  s_name = text_layer_create(
    GRect(60, 21, 72, 30));
  s_number = text_layer_create(
    GRect(22, 68, 40, 30));
  s_slp_layer = text_layer_create(
    GRect(115,60,40,30));
  s_level = text_layer_create(
    GRect(108,29,40,30));
  s_batt = text_layer_create(
    GRect(86,45,40,30));

  //Makes layout more watchface-like
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, pokegb_font);

  //Date label set
  text_layer_set_background_color(s_d_label, GColorClear);
  text_layer_set_text_color(s_d_label, GColorBlack);
  text_layer_set_font(s_d_label, pokegb_fsmall);
  text_layer_set_background_color(s_m_label, GColorClear);
  text_layer_set_text_color(s_m_label, GColorBlack);
  text_layer_set_font(s_m_label, pokegb_fsmall);
  text_layer_set_background_color(s_w_label, GColorClear);
  text_layer_set_text_color(s_w_label, GColorBlack);
  text_layer_set_font(s_w_label, pokegb_fsmall);
  text_layer_set_background_color(s_y_label, GColorClear);
  text_layer_set_text_color(s_y_label, GColorBlack);
  text_layer_set_font(s_y_label, pokegb_fsmall);

  //Pokemon ID
  text_layer_set_background_color(s_name, GColorClear);
  text_layer_set_text_color(s_name, GColorBlack);
  text_layer_set_font(s_name, pokegb_fsmall);

  text_layer_set_background_color(s_number, GColorClear);
  text_layer_set_text_color(s_number, GColorBlack);
  text_layer_set_font(s_number, pokegb_fsmall);
  
  //Sleep/Level/Battery Status
  text_layer_set_background_color(s_slp_layer, GColorClear);
  text_layer_set_text_color(s_slp_layer, GColorBlack);
  text_layer_set_font(s_slp_layer, pokegb_fsmall);
  text_layer_set_background_color(s_level, GColorClear);
  text_layer_set_text_color(s_level, GColorBlack);
  text_layer_set_font(s_level, pokegb_fsmall);
  text_layer_set_background_color(s_batt, GColorClear);
  text_layer_set_text_color(s_batt, GColorBlack);
  text_layer_set_font(s_batt, pokegb_fsmall);

  
  //Create GBitmap
  bg_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MENU);

  //Create BitmapLayer to display GBitmap
  mon_layer = bitmap_layer_create(GRect(5,5,56,56));
  bg_layer = bitmap_layer_create(bounds);

  //Set the bitmap onto layer and add to window
  bitmap_layer_set_bitmap(bg_layer, bg_bitmap);
  bitmap_layer_set_bitmap(mon_layer, mon_bitmap);
  layer_add_child(w_layer, bitmap_layer_get_layer(bg_layer));
  layer_add_child(w_layer, bitmap_layer_get_layer(mon_layer));

  //Add it as a child layer to the Window's root layer
  layer_add_child(w_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(w_layer, text_layer_get_layer(s_d_label));
  layer_add_child(w_layer, text_layer_get_layer(s_m_label));
  layer_add_child(w_layer, text_layer_get_layer(s_w_label));
  layer_add_child(w_layer, text_layer_get_layer(s_y_label));
  layer_add_child(w_layer, text_layer_get_layer(s_name));
  layer_add_child(w_layer, text_layer_get_layer(s_number));
  layer_add_child(w_layer, text_layer_get_layer(s_slp_layer));
  layer_add_child(w_layer, text_layer_get_layer(s_level));
  layer_add_child(w_layer, text_layer_get_layer(s_batt));

  handle_battery(battery_state_service_peek());
}

static void mw_unload(Window *window){
  //Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_d_label);
  text_layer_destroy(s_m_label);
  text_layer_destroy(s_w_label);
  text_layer_destroy(s_y_label);
  text_layer_destroy(s_name);
  text_layer_destroy(s_number);
  text_layer_destroy(s_slp_layer);
  text_layer_destroy(s_level);
  text_layer_destroy(s_batt);

  //Destroy GBitmap
  gbitmap_destroy(bg_bitmap);

  //Destroy BitmapLlayer
  bitmap_layer_destroy(mon_layer);
  bitmap_layer_destroy(bg_layer);
}

static void init(){
   //Registering with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  //Registering with ConnectionService
  connection_service_subscribe((ConnectionHandlers) {
  .pebble_app_connection_handler = app_connection_handler,
  .pebblekit_connection_handler = kit_connection_handler
});

  battery_state_service_subscribe(handle_battery);

  //Create main Window element and assign to pointer
  s_main_window = window_create();

  //Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = mw_load,
    .unload = mw_unload
  });

  //Show Window on the watch; animated = true
  window_stack_push(s_main_window, true);

  //Make sure time is displayed from the start
  update_time();
  update_poke();
  handle_battery(battery_state_service_peek());
  //check_level();

}

static void deinit(){
  //Destroy Window
  window_destroy(s_main_window);

}

int main(void){
  init();
  app_event_loop();
  deinit();
}