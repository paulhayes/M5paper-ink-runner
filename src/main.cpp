#include <Arduino.h>
#include <Esp.h>
#include <M5EPD.h>
#include "FS.h"
#include "SPIFFS.h"
#include "gui-utils.h"
#include "ink-player.h"

char* story_filename;

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
    
    setup_gui();

    // auto h = canvas.fontHeight();
    
    // canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);

    select_file();
    contine_story();
}


void loop()
{
    delay(20);
    M5.update();
    if(get_num_choices()==0){
        if(M5.BtnP.wasPressed()){
            story_filename=select_file();
            load_story(story_filename);
            free(story_filename);  
            contine_story();
        }
    }
    else if(check_selection()){
        Serial.print("> user selected ");
        Serial.println(get_current_choice());        
        story_choice(get_current_choice());
        clear_choices();
        contine_story();
    }
    
}
