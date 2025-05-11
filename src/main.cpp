#include <Arduino.h>
#include <Esp.h>
#include <M5EPD.h>
#include "FS.h"
#include "SPIFFS.h"
#include "gui-utils.h"
#include "ink-player.h"
#include "page.hpp"
#ifndef PIO_UNIT_TESTING
char* story_filename;

M5EPD_Canvas canvas(&M5.EPD);
M5EPD_Canvas selected_icon(&M5.EPD);
M5EPD_Canvas unselected_icon(&M5.EPD);

Paginator paginator = Paginator(canvas);

void select_story();

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

    M5.begin(true,false,true,false,false);
    M5.EPD.SetRotation(90);
    M5.TP.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();
    
    //load_font("/Roboto-Black.ttf");
    
    setup_gui(&canvas,&selected_icon,&unselected_icon);

    select_story();
}


int selected_choice = -1;
void loop()
{
    delay(20);
    M5.update();
    if(!paginator.hasChoices()){
        if(M5.BtnP.wasPressed()){
            select_story();
        }
    }
    else if(check_selection(paginator,canvas,selected_icon,unselected_icon,selected_choice)){
        
        Serial.print("> user selected ");
        Serial.println(selected_choice); 
        story_choice(selected_choice);
        selected_choice = -1;
        //clear_choices();
        contine_story(paginator);
        draw_selection_cursor(paginator,selected_icon,unselected_icon,selected_choice);       
        Serial.print("free mem:");
        Serial.println(heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    }
    
    
}

void select_story()
{
    //gui_clear(canvas);
    paginator.clear();
    story_filename=select_file(canvas, paginator, "Select Story",selected_icon,unselected_icon);
    load_story(story_filename);
    free(story_filename);
    contine_story(paginator);
    draw_selection_cursor(paginator,selected_icon,unselected_icon,selected_choice);
        
}

#endif