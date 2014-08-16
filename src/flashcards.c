#include <pebble.h>

#define NUM_MENU_SECTIONS 1

#define NUM_FIRST_MENU_ITEMS 1
#define NUM_SECOND_MENU_ITEMS 1

static Window *window;
static Window *windowCard;

static TextLayer* txtlyer;
static InverterLayer* invert;
// This is a menu layer
// You have more control than with a simple menu layer
static MenuLayer *menu_layer;

// Menu items can optionally have an icon drawn with them


static int current_icon = 0;

// You can draw arbitrary things in a menu item such as a background
static GBitmap *menu_background;

// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections

int num_cards = 0;
char**namesdata;
char**cardsdata;
char**cdarray;
int num_items=0;
void putCard();

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}


 void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"Failed to send");
 }

int sent_cards=0;
int sent_items=0;
 void in_received_handler(DictionaryIterator *received, void *context) {
  Tuple *text_tuple = dict_find(received, 0);
  int valu=text_tuple->value->uint8;
  if(valu==0){
     
     Tuple *index_tuple = dict_find(received, 1);
     Tuple *cards_tuple = dict_find(received, 2);
     Tuple *name_tuple = dict_find(received, 3);
     int inx=index_tuple->value->uint8;
     //APP_LOG(APP_LOG_LEVEL_DEBUG,"Received stack: %d", inx);
     namesdata[inx]=malloc(strlen(name_tuple->value->cstring)+1);
     cardsdata[inx]=malloc(strlen(cards_tuple->value->cstring)+1);
     strcpy(namesdata[inx], name_tuple->value->cstring);
     strcpy(cardsdata[inx], cards_tuple->value->cstring);
     sent_items++;
     if(sent_items==num_items){
      //APP_LOG(APP_LOG_LEVEL_DEBUG,"Received stacks");
      menu_layer_reload_data(menu_layer);
     }
  }else if(valu==2){
    sent_items=0;

    //APP_LOG(APP_LOG_LEVEL_DEBUG,"Received stack primer");
    if(num_items>0){
      for (int i = 0; i < num_items; ++i)
      {
        free(namesdata[i]);
        free(cardsdata[i]);
      }
      free(namesdata);
      free(cardsdata);

    }
    Tuple *index_tuple = dict_find(received, 1);
    int lengt=index_tuple->value->uint8;
    num_items=lengt;
    namesdata=malloc(lengt*sizeof(char*));
    cardsdata=malloc(lengt*sizeof(char*));


  } if(valu==1){
    
    Tuple *index_tuple = dict_find(received, 1);
    Tuple *qa_tuple = dict_find(received, 2);
    int index=qa_tuple->value->uint8;
    if(index==2){
      //Length
      sent_cards=0;
      uint8_t numQs=index_tuple->value->uint8;
      num_cards=numQs;
      cdarray=malloc(numQs*2*sizeof(char*));
      //APP_LOG(APP_LOG_LEVEL_DEBUG,"Allocated array of size: %d", numQs*2*sizeof(char*));
    }else{
    Tuple *text_tuple = dict_find(received, 3);
    //APP_LOG(APP_LOG_LEVEL_DEBUG,"Starting...");
    int indval=index_tuple->value->uint8;

    cdarray[indval]=malloc(strlen(text_tuple->value->cstring)+1);
    sent_cards++;
    strcpy(cdarray[indval],text_tuple->value->cstring);
    //APP_LOG(APP_LOG_LEVEL_DEBUG,"Received text: %s at %d, sentcards=%d", cdarray[indval], indval,sent_cards);

    }
    if(sent_cards>=num_cards*2){
      //APP_LOG(APP_LOG_LEVEL_DEBUG,"Completed!");
      putCard();
    }
  }
 }

 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }



void uninvertText(){
  layer_remove_child_layers(text_layer_get_layer(txtlyer));
}
void invertText(){
  layer_add_child(text_layer_get_layer(txtlyer), inverter_layer_get_layer(invert));
}

void putMenu(){

}
int i=0;
int inv=1;
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!inv){
    invertText();
    inv = 1;
  }else{
    uninvertText();
    inv=0;
  }
  i++;
//  //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", i);
  if(i >= num_cards*2){
    i=0;
  }
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"ACCESSED: text: %s at %d",cdarray[i],i);
  text_layer_set_text(txtlyer, cdarray[i]);
  

}


void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  // Process tap on ACCEL_AXIS_X, ACCEL_AXIS_Y or ACCEL_AXIS_Z
  // Direction is 1 or -1
  if (!inv){
    invertText();
    inv = 1;
  }else{
    uninvertText();
    inv=0;
  }
  i++;
//  //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", i);
  if(i >= num_cards*2){
    i=0;
  }
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"ACCESSED: text: %s at %d",cdarray[i],i);
  text_layer_set_text(txtlyer, cdarray[i]);
}



static void back_click_handler(ClickRecognizerRef recognizer, void *context){
    uninvertText();
    inv=0;
    i=0;
    num_cards=0;
    for (int i = 0; i < num_cards*2; ++i){
      free(cdarray[i]);
    }
    free(cdarray);
    accel_tap_service_unsubscribe();
    window_stack_pop(true); 
    return;

}


static void up_click_handler(ClickRecognizerRef recognizer, void *context) {

  if(inv){
    uninvertText();
    inv=0;
  }
  if(i%2 == 0){
    //on a question.
    i-=2;
  }else{
    //On an answer
    i-=3; // Get back to a different question
  }

  if(i < 0 ){
    i= (num_cards*2)-2;
  }
    //APP_LOG(APP_LOG_LEVEL_DEBUG,"ACCESSED: text: %s at %d",cdarray[i],i);
    text_layer_set_text(txtlyer, cdarray[i]);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {

  if(inv){
    uninvertText();
    inv=0;
  }
  if(i%2 == 0){
    //on a question.
    i+=2;
  }else{
    //On an answer
    i++;
  }

  if(i >= num_cards*2){
    i=0;
  }


    //APP_LOG(APP_LOG_LEVEL_DEBUG,"ACCESSED: text: %s at %d",cdarray[i],i);
    text_layer_set_text(txtlyer, cdarray[i]);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}

void putCard(){
  accel_tap_service_subscribe(accel_tap_handler);
  window_stack_push(windowCard, true /* Animated */);
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"ACCESSED: text: %s at %d",cdarray[0],0);
  text_layer_set_text(txtlyer, cdarray[0]);
  uninvertText();
  inv=0;
}


// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return num_items;
}

// A callback is used to specify the height of the section header
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
menu_cell_basic_header_draw(ctx, cell_layer, "Quiz Deck");
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  //cell_index->row;

  menu_cell_basic_draw(ctx, cell_layer, namesdata[cell_index->row], cardsdata[cell_index->row], NULL);

}

int cur=0;
// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action

  DictionaryIterator* dictionaryIterator = NULL;
  app_message_outbox_begin(&dictionaryIterator);

  Tuplet tuple = TupletInteger(0x04, cell_index->row);
  dict_write_tuplet(dictionaryIterator, &tuple);
  dict_write_end (dictionaryIterator);
  app_message_outbox_send();
}


// This initializes the menu upon window load
void window_load(Window *window) {
  // Here we load the bitmap assets
  // resource_init_current_app must be called before all asset loading


  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  //window_set_click_config_provider(window, click_config_provider);
  // Create the menu layer
  menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  
  
  menu_layer_set_click_config_onto_window(menu_layer, window);
  
  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));

  //layer_add_child(window_layer, text_layer_get_layer(txtlyer));
  
}

void card_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  invert = inverter_layer_create  (bounds);
  GFont *font = fonts_get_system_font (FONT_KEY_GOTHIC_28); 
  txtlyer = text_layer_create(bounds);
  text_layer_set_font(txtlyer, font);
  text_layer_set_text(txtlyer, "Testing Text");
  window_set_click_config_provider(windowCard, click_config_provider);
  layer_add_child(window_layer, text_layer_get_layer(txtlyer));
}
void card_unload(Window *window){

}
void window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);


  // And cleanup the background
  gbitmap_destroy(menu_background);
}
void init_messages(){
   app_message_register_inbox_received(in_received_handler);
   app_message_register_inbox_dropped(in_dropped_handler);
   app_message_register_outbox_sent(out_sent_handler);
   app_message_register_outbox_failed(out_failed_handler);

   app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}
int main(void) {

  init_messages();
  window = window_create();
  windowCard = window_create();

  window_set_window_handlers(windowCard, (WindowHandlers){
    .load = card_load,
    .unload = card_unload,
  });
  // Setup the window handlers
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  
  window_stack_push(window, true /* Animated */);
  app_event_loop();

  window_destroy(window);
}
