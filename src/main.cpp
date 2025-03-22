#include <Arduino.h>
#include <Esp.h>
#include <M5EPD.h>
#include "FS.h"
#include "SPIFFS.h"
#include <story.h>
#include <runner.h>
#include <choice.h>

using namespace ink::runtime;
ink::runtime::globals globalVars;

M5EPD_Canvas canvas(&M5.EPD);

story *myInk;
runner _thread;

int cursorX = 0;
int cursorY = 0;
int padding = 5;
int current_indent = padding;

int choice_positions[10];
int num_choice_pos = 0;
int current_choice = 0;


void next_line()
{
    cursorY += canvas.fontHeight();
    cursorX = current_indent;
}

const char *strnchr(const char* ptr, int ch, size_t size) {
    
    while (size > 0) {
      size--;
      if (ptr[size] == (char) ch) {
        return ptr+size;
      } 
      if (ptr[size] == '\0') {
        break;
      } 
    }
    return NULL;
  }

void word_wrap(const char *line_c)
{
    //Serial.println("word_wrap start");
    int len = strlen(line_c);
    char *all = (char *)malloc(len + 1);
    const char *current = all;
    char *line = (char*)malloc(len+1);
    strcpy(all, line_c);
    const char *remaining = current;
    int i = 0;
    do
    {
        strcpy(line,current);
        remaining = current + strlen(current);
        int search_len = strlen(current);
        while ((canvas.width() - (padding + cursorX)) < canvas.textWidth(line))
        {
            //Serial.print("Finding last space in :");
            //Serial.println(current);
            const char *last = strnchr(current, ' ', search_len);
            if (last == NULL)
            {
                last = current + search_len - 1;
                Serial.println("substract character");
            }
            else {
                Serial.println("substract word");
            }            
            search_len = last-current;
            //strncpy(line,current,search_len);
            line[search_len]='\0';
            remaining = last + 1;
            // Serial.print("Testing:");
            // Serial.println(line);
        }
        if (current != NULL && strlen(current) > 0)
        {
            // Serial.print("At ");
            // Serial.print(cursorX);
            // Serial.print(",");
            // Serial.print(cursorY);
            // Serial.print(" ");
            // Serial.println(line);
            canvas.drawString(line, cursorX, cursorY);
        }
        next_line();
        current = remaining;
        // Serial.print("S ");
        // Serial.print((int)remaining);
        // Serial.print(" ");
        // Serial.print((int)(all + len));
        // Serial.println(" ");
        if ((i++) > 5)
        {
            break;
        }
        //Serial.println("next line");
    } while (remaining < all + len);
    free(all);
    //Serial.println("word_wrap end");
}

void write_paragraph()
{
    while (_thread->can_continue() && cursorY<(canvas.height()-canvas.fontHeight()))
    {
        // Serial.print("Free heap size ");
        // Serial.println(ESP.getFreeHeap());

        auto line_c = _thread->getline_alloc();

        if (line_c != NULL)
        {
            Serial.println(line_c);
            word_wrap(line_c);
        }
        else
        {
            Serial.println("line was null");
        }
        next_line();
        // Serial.print("Free heap size ");
        // Serial.println(ESP.getFreeHeap());//this is the same as before running getline. Surely some memmory allocation should happen?
    }
}

void write_choices()
{
    next_line();
    // Iterate choices
    if (_thread->has_choices())
    {
        Serial.print("num choices");
        Serial.println(_thread->num_choices());
        int num_choices = _thread->num_choices();
        num_choice_pos = num_choices;
        for (int i = 0; i < num_choices; i++)
        {
            auto text = _thread->get_choice(i)->text();
            //Serial.println(text);
            Serial.println(cursorY);
            choice_positions[i] = cursorY;
            word_wrap(text);
            next_line();
        }
    }
    else {
        num_choice_pos = 0;
    }
}

void draw_selection_cursor()
{
    for(int i=0;i<num_choice_pos;i++){
        canvas.setTextColor(i==current_choice?15:0);
        //canvas.drawChar('>',0,choice_positions[i]);
        canvas.drawString(">",0,choice_positions[i]);
    }
    canvas.setTextColor(15);
    //canvas.drawString("moop",0,100);
    Serial.println("pushCanvas UPDATE_MODE_DU4");
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
}

void redraw()
{
    Serial.println("redraw");
    cursorX = current_indent = padding;
    cursorY = 10;
    write_paragraph();
    current_indent = 20;
    next_line();
    write_choices();
    draw_selection_cursor();
    Serial.println("Redraw complete");
    //canvas.drawString("Hello World", 0, 0);
    //canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
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

    File file = SPIFFS.open("/story.bin");
    const int test_story_bin_len = file.size();
    char* test_story_bin = (char*)malloc(test_story_bin_len);
    file.readBytes(test_story_bin,test_story_bin_len);
    myInk = story::from_binary((unsigned char*)(test_story_bin), test_story_bin_len, false);

    // Create a new thread
    _thread = myInk->new_runner(globalVars);
    //_thread = thread;
    if (!_thread.is_valid())
    {
        Serial.println("Invalid binary story file");
        return;
    }

    Serial.println("Test Done");

    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();
    canvas.createCanvas(540, 960);
    // auto h = canvas.fontHeight();
    canvas.setTextSize(3);
    // canvas.drawString("Hello World", 0, 0);
    // canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);

    redraw();
}

void loop()
{
    if( M5.BtnL.wasPressed() ){
        Serial.println('-');
        current_choice--;
        if(current_choice<0){
            current_choice=num_choice_pos-1;
        }
        draw_selection_cursor();
    }
    if(M5.BtnR.wasPressed()){
        Serial.println('+');
        current_choice++;
        if(current_choice>=num_choice_pos){
            current_choice=0;
        }
        draw_selection_cursor();
    }
    if(M5.BtnP.wasPressed() && num_choice_pos>0){
        if(num_choice_pos>0){
            Serial.print("> user selected ");
            Serial.println(current_choice);        
            _thread->choose(current_choice);
            current_choice=0;
            num_choice_pos=0;    
        }
        else {
            Serial.print("next");
        }
        canvas.clear();
        redraw();
        
    }
    delay(10);
    M5.update();
    // while running
    // while can story continue
    // get next paragraph of story
    // get tags
    // pagnate content where neccessary
    // end while
    // wait for user input choice
    // select choice

    // endwhile
}
