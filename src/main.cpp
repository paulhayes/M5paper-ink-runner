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

void select_story()
{
    clear_choices();
    gui_clear(canvas);
    story_filename=select_file("Select Story",paginator);
    load_story(story_filename);
    free(story_filename);  
    contine_story();
}

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

    M5.begin();
    M5.EPD.SetRotation(90);
    M5.TP.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();
    
    //load_font("/Roboto-Black.ttf");
    
    setup_gui();

    select_story();
}


void loop()
{
    delay(20);
    M5.update();
    if(!paginator.hasChoices()){
        if(M5.BtnP.wasPressed()){
            select_story();
        }
    }
    else if(check_selection(canvas, paginator)){
        
        Serial.print("> user selected ");
        Serial.println(get_current_choice());        
        story_choice(get_current_choice());
        //clear_choices();
        contine_story();
    }
    
}
#endif