#include <M5EPD.h>
#include <story.h>
#include <runner.h>
#include <choice.h>
#include "gui-utils.h"
#include "ink-player.h"
#include "FS.h"
#include "SPIFFS.h"

using namespace ink::runtime;
ink::runtime::globals global_vars;

story *myInk;
runner _thread;

void contine_story()
{
    Serial.println("contine story");
    gui_clear();
    
    write_story_section();
    set_indent(40);
    next_line();
    write_choices();
    //canvas.drawString("Hello World", 0, 0);
    gui_draw();
    draw_selection_cursor();
    
    Serial.println("Redraw complete");
}

void write_story_section()
{
    while (_thread->can_continue() && cursor_inside_canvas())
    {
        // Serial.print("Free heap size ");
        // Serial.println(ESP.getFreeHeap());

        auto line_c = _thread->getline_alloc();
        
        if (line_c != NULL)
        {
            Serial.println(line_c);
            auto remaining = word_wrap(line_c);
            
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
    clear_choices();
    // Iterate choices
    if (_thread->has_choices())
    {
        
        Serial.print("num choices");
        Serial.println(_thread->num_choices());
        int num_choices = _thread->num_choices();
        for (int i = 0; i < num_choices; i++)
        {
            auto text = _thread->get_choice(i)->text();
            //Serial.println(text);
            Serial.print(i);
            Serial.print(" ");
            Serial.println(text);
            add_choice_option(text);
            next_line();
        }
    }
    
}

void story_choice(int choice)
{
    _thread->choose(choice);
}

void load_story(const char* story_filename)
{
    Serial.print("Opening ");
    Serial.println(story_filename);
    File file = SPIFFS.open(story_filename);
    const int test_story_bin_len = file.size();
    char* test_story_bin = (char*)malloc(test_story_bin_len);
    file.readBytes(test_story_bin,test_story_bin_len);
    if(myInk){
        delete myInk;
        //delete _thread.get();
    }
    myInk = story::from_binary((unsigned char*)(test_story_bin), test_story_bin_len, true);
    //global_vars = ink::runtime::globals();
    // Create a new thread

    _thread = myInk->new_runner();
    //_thread = thread;
    if (!_thread.is_valid())
    {
        Serial.println("Invalid binary story file");        
    }
    file.close();
    
}