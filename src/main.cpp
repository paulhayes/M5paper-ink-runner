#include <Arduino.h>
#include <Esp.h>
#include <M5EPD.h>
#include <story.h>
#include <runner.h>
#include <choice.h>
#include "teststory.h"

using namespace ink::runtime;
ink::runtime::globals globalVars;

M5EPD_Canvas canvas(&M5.EPD);

story* myInk;
runner _thread;
void setup()
{
    Serial.begin(115200);
    
    Serial.println("Testing...");
    myInk = story::from_binary(test_story_bin,test_story_bin_len,false);
    
    // Create a new thread
    _thread = myInk->new_runner(globalVars);
    //_thread = thread;
    if(!_thread.is_valid()){ 
        Serial.println("Invalid binary story file");
        return;
    }

    while(_thread->can_continue()){
        // Serial.print("Free heap size ");
        // Serial.println(ESP.getFreeHeap());
                
        auto line_c = _thread->getline_alloc();

        if(line_c!=NULL){
            Serial.println(line_c);
        }
        else {
            Serial.println("line was null");
        }
        // Serial.print("Free heap size ");
        // Serial.println(ESP.getFreeHeap());//this is the same as before running getline. Surely some memmory allocation should happen?
        
    }

    // Iterate choices
    if(_thread->has_choices()){
        Serial.print("num choices");
        Serial.println(_thread->num_choices()); 
        int num_choices = _thread->num_choices();           
        for(int i=0;i<num_choices;i++) {
            Serial.println(_thread->get_choice(i)->text());
        }
        _thread->choose(0);    
    }
    Serial.println("Test Done");

    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();
    canvas.createCanvas(540, 960);
    canvas.setTextSize(3);
    canvas.drawString("test", 45, 350);    
    canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
}

void loop()
{
    //load content
    //while running
        //while can story continue
            //get next paragraph of story
            //get tags
            //pagnate content where neccessary
        //end while
        //wait for user input choice
        //select choice
        
    //endwhile
}
