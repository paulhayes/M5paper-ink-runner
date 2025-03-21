#include <Arduino.h>
#include <M5EPD.h>
// #include <story.h>
// #include <runner.h>
// #include <choice.h>

M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();
    canvas.createCanvas(540, 960);
    canvas.setTextSize(3);
    canvas.drawString("Hello Lisbon", 45, 350);
    canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
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
