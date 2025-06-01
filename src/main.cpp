#include <Arduino.h>
#include <Esp.h>
#include "FS.h"
#include "SPIFFS.h"
#include "main.hpp"
#include "ink-player.h"

#ifndef PIO_UNIT_TESTING



void setup()
{    
    Serial.begin(115200);
    
    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS Mount Failed");
        return;
      }

    
    Serial.println("Testing...");
    // load content

    Serial.println("Test Done");

    M5.begin(true,true,true,true,false);
    M5.EPD.SetRotation(90);
    M5.TP.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();
    
    //load_font("/Roboto-Black.ttf");
    
    setup_gui(gui_elements);

    select_story();
}


int selected_choice = -1;
void loop()
{
    delay(20);
    M5.update();
    if(check_selection(gui_elements,selected_choice)){
        if(selected_choice==-1){
            if(!paginator.hasChoices()){
                select_story();
            }
            return;
        }
     
        Serial.print("> user selected ");
        Serial.println(selected_choice); 
        story_choice(selected_choice);
        selected_choice = -1;
        //clear_choices();
        contine_story(paginator);
        menu_bar_draw(gui_elements); 
        draw_selection_cursor(gui_elements,selected_choice);       
        Serial.print("free mem:");
        Serial.println(heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    }    
}

void select_story()
{
    //gui_clear(canvas);
    paginator.marginTop=5;
    paginator.clear();    
    menu_bar_draw(gui_elements);
    story_filename=select_file(gui_elements, "Select Story");
    load_story(story_filename);
    free(story_filename);
    contine_story(paginator);
    menu_bar_draw(gui_elements); 
    draw_selection_cursor(gui_elements,selected_choice);
        
}

#endif