#include <Arduino.h>
#include <M5EPD.h>
#include <story.h>
#include <runner.h>
#include <choice.h>
#include "teststory.h"
#include <Esp.h>

using namespace ink::runtime;
ink::runtime::globals globalVars;

M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    Serial.begin(115200);
    
    Serial.println("Testing...");
    story* myInk = story::from_binary(test_story_bin,test_story_bin_len);
    
    // Create a new thread
    runner thread = myInk->new_runner();
    if(!thread.is_valid()){ 
        Serial.println("Invalid binary story file");
    }

    std::string line = std::string("unpopulated");
    
    if(thread.is_valid() && thread->can_continue()){
        Serial.print("Free heap size ");
        Serial.println(ESP.getFreeHeap());
        std::string line2 = thread->getline();
        Serial.print("pointer ");
        Serial.println((int)(line2.c_str()), 16); //line2 pointer is NULL
        Serial.print("Free heap size ");
        Serial.println(ESP.getFreeHeap());
        Serial.println(line2.c_str()); //this is the same as before running getline. Surely some memmory allocation should happen?
    }

    // M5.begin();
    // M5.EPD.SetRotation(90);
    // M5.EPD.Clear(true);
    // M5.RTC.begin();
    // canvas.createCanvas(540, 960);
    // canvas.setTextSize(3);
    // canvas.drawString(line.c_str(), 45, 350);    
    // canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
}

void loop()
{
    //load content
    //while running
        //while can stroy continue
            //get next paragraph of story
            //get tags
            //pagnate content where neccessary
        //end while
        //wait for user input choice
        //select choice
        
    //endwhile
}
