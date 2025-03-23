#include <Arduino.h>
#include <Esp.h>
#include <M5EPD.h>
#include "FS.h"
#include "SPIFFS.h"
#include <story.h>
#include <runner.h>
#include <choice.h>

using namespace ink::runtime;
ink::runtime::globals global_vars;

M5EPD_Canvas canvas(&M5.EPD);
M5EPD_Canvas selected_icon(&M5.EPD);
M5EPD_Canvas unselected_icon(&M5.EPD);

story *myInk;
runner _thread;

int cursor_x = 0;
int cursor_y = 0;
int padding = 5;
int current_indent = padding;

int choice_positions[10];
int num_choice_pos = 0;
int current_choice = 0;

char* story_filename;

void next_line()
{
    cursor_y += canvas.fontHeight();
    cursor_x = current_indent;
}

void draw_selection_cursor()
{
    for(int i=0;i<num_choice_pos;i++){
        M5EPD_Canvas* icon = (i==current_choice)? &selected_icon: &unselected_icon;
        icon->pushCanvas(5,choice_positions[i],UPDATE_MODE_DU4);
    }
}

bool check_selection()
{
    M5.TP.update();
    if(M5.TP.available() && !M5.TP.isFingerUp()){
        
        tp_finger_t finger = M5.TP.readFinger(0);
        // Serial.print("x=");
        // Serial.print(finger.x);
        // Serial.print("y=");
        // Serial.print(finger.y);
        // Serial.println("");

        for(int i=0;i<num_choice_pos;i++){
            int maxY = choice_positions[i]+canvas.fontHeight();
            bool inY = finger.y>choice_positions[i] && finger.y<maxY;
            if(inY){
                current_choice=i;
                draw_selection_cursor();
                while(!M5.TP.isFingerUp()){
                    finger = M5.TP.readFinger(0);
                    delay(5);
                    M5.TP.update();
                }
                inY = finger.y>choice_positions[i] && finger.y<maxY;
                if(inY){
                    Serial.println("finger up inside of bounds");
                    return true;
                }
                Serial.println("finger up outside of bounds");
                return false;
            }
        }
    }
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
    if(M5.BtnP.wasPressed()){
        Serial.println("button down");
        return true;
    }
    return false;
}


void select_file()
{
    canvas.clear();
    canvas.drawString("Select Story", 0, 0);
    File root = SPIFFS.open("/");
    File file;
    char *files[10];
    int num_files=0;
    while(file=root.openNextFile()){
        const char* name=file.name();
        int len=strlen(name);
        if( strcmp(name+len-4,".bin")==0 ){
            Serial.println(name);
            files[num_files]=(char*)malloc(len);
            strcpy(files[num_files],name);
            int y=100+num_files*2*canvas.fontHeight();
            choice_positions[num_files]=y;
            //canvas.drawCentreString(name,canvas.width()/2,y,1);
            canvas.drawString(name,100,y);
            num_files++;
        }
        file.close();
    }
    
    num_choice_pos=num_files;
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
    draw_selection_cursor();
    M5.update();
    while(!check_selection()){
        M5.update();
        delay(10);
    }
    Serial.print("Option ");
    Serial.print(current_choice);
    Serial.print(" ");
    Serial.print(files[current_choice]);
    Serial.println(" selected");
    const char* filename = files[current_choice];
    int filename_len = strlen(filename);
    story_filename = (char*)malloc(filename_len+2);
    story_filename[0]='/';
    strcpy(story_filename+1,filename);

    current_choice=0;
    num_choice_pos=0;

    Serial.print("Opening ");
    Serial.println(story_filename);
    Serial.println(filename);
    file = SPIFFS.open(story_filename);
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
    free(story_filename);
    file.close();
    root.close();
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
        while ((canvas.width() - (padding + cursor_x)) < canvas.textWidth(line))
        {
            //Serial.print("Finding last space in :");
            //Serial.println(current);
            const char *last = strnchr(current, ' ', search_len);
            if (last == NULL)
            {
                last = current + search_len - 1;
                //Serial.println("substract character");
            }
            else {
                //Serial.println("substract word");
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
            canvas.drawString(line, cursor_x, cursor_y);
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
    while (_thread->can_continue() && cursor_y<(canvas.height()-canvas.fontHeight()))
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
            Serial.print(i);
            Serial.print(" ");
            Serial.println(text);
            choice_positions[i] = cursor_y;
            word_wrap(text);
            next_line();
        }
    }
    else {
        num_choice_pos = 0;
    }
}



void redraw()
{
    Serial.println("redraw");
    canvas.clear();
    cursor_x = current_indent = padding;
    cursor_y = 10;
    write_paragraph();
    current_indent = 40;
    next_line();
    write_choices();
    //canvas.drawString("Hello World", 0, 0);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
    draw_selection_cursor();
    
    Serial.println("Redraw complete");
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
    canvas.createCanvas(540, 960);
    canvas.setTextSize(3);
    int icon_w=canvas.textWidth(">");
    int icon_h=canvas.fontHeight();
    selected_icon.createCanvas(icon_w,icon_h);
    unselected_icon.createCanvas(icon_w,icon_h);
    selected_icon.setTextSize(3);
    selected_icon.setTextColor(15);
    selected_icon.drawString(">",0,0);
    unselected_icon.setTextSize(3);
    unselected_icon.setTextColor(0);
    unselected_icon.drawString(">",0,0);

    // auto h = canvas.fontHeight();
    
    // canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);

    select_file();
    redraw();
}


void loop()
{
    delay(20);
    M5.update();
    if(num_choice_pos==0){
        if(M5.BtnP.wasPressed()){
            select_file();
            redraw();        
        }
    }
    else if(check_selection()){
        Serial.print("> user selected ");
        Serial.println(current_choice);        
        _thread->choose(current_choice);
        current_choice=0;
        num_choice_pos=0;
        redraw();
    }
    
}
