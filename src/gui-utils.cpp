#include <M5EPD.h>
#include "gui-utils.h"

M5EPD_Canvas canvas(&M5.EPD);
M5EPD_Canvas selected_icon(&M5.EPD);
M5EPD_Canvas unselected_icon(&M5.EPD);


int choice_positions[10];
int num_choice_pos = 0;
int current_choice = 0;


int cursor_x = 0;
int cursor_y = 0;
int padding = 5;
int current_indent = padding;

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

char* select_file()
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
            add_choice_option(name,100,y);
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
    root.close();
    Serial.print("Option ");
    Serial.print(current_choice);
    Serial.print(" ");
    Serial.print(files[current_choice]);
    Serial.println(" selected");
    const char* filename = files[current_choice];
    int filename_len = strlen(filename);
    char* story_filename = (char*)malloc(filename_len+2);
    story_filename[0]='/';
    strcpy(story_filename+1,filename);

    current_choice=0;
    num_choice_pos=0;

    return story_filename;
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
        
        //Serial.println("next line");
    } while (remaining < all + len);
    free(all);
    //Serial.println("word_wrap end");
}

bool cursor_inside_canvas()
{
    return cursor_y<(canvas.height()-canvas.fontHeight());
}

void copyRect(M5EPD_Canvas *src, M5EPD_Canvas *dst, int w, int h)
{
    for(int x=0;x<w;x++){
        for(int y=0;y<h;y++){
            dst->drawPixel( x,y,src->readPixel(x,y) );
        }
    }
}

void setup_gui()
{
    const int textSize=2;
    canvas.createCanvas(540, 960);
    canvas.setTextSize(textSize);
    canvas.setTextFont(2);
    int icon_w=canvas.textWidth(">");
    int icon_h=canvas.fontHeight();
    selected_icon.createCanvas(icon_w,icon_h);
    unselected_icon.createCanvas(icon_w,icon_h);
    canvas.setTextColor(15);
    canvas.drawString(">",0,0);
    copyRect(&canvas,&selected_icon,icon_w,icon_h);
    canvas.setTextColor(0);
    canvas.drawString(">",0,0);
    canvas.setTextColor(15);
    copyRect(&canvas,&unselected_icon,icon_w,icon_h);
}



void gui_clear()
{
    canvas.clear();
    cursor_x = current_indent = padding;
    cursor_y = 10;
}

void gui_draw()
{
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU);
}

int get_num_choices()
{
    return num_choice_pos;
}

int get_current_choice()
{
    return current_choice;
}

void add_choice_option(const char* name)
{
    // choice_positions[i] = cursor_y;
    // word_wrap(text);
    add_choice_option(name,cursor_x,cursor_y);
}

void add_choice_option(const char* text,int x, int y)
{
    choice_positions[num_choice_pos]=y;
    //canvas.drawString(name,100,y);
    cursor_x=x;
    cursor_y=y;
    word_wrap(text);
    num_choice_pos++;
}
void clear_choices(){
    num_choice_pos=0;
    current_choice=0;
}

void set_indent(int indent)
{
    current_indent = indent;
}

void set_font(const char* fontFilename)
{
    canvas.loadFont(fontFilename,SPIFFS);
    canvas.setTextColor(15);
    canvas.createRender(3);
    canvas.createRender(32,256);
}