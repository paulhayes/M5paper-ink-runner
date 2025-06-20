#include <functional>
#include <cstring>
#include "page.hpp"
#include "gui-utils.h"

Page::~Page()
{
    for(int i=numCopySections-1;i>=0;i--){
        // Serial.print("Deleting :");
        // Serial.println(copySections[i].text);
        free((void*)copySections[i].text);
    }
}

void Page::addLine(char *copy, int x, int y)
{    
    int lineAddr = *copy;    
    copySections[numCopySections] = {copy,x,y};
    numCopySections++;
}

void Page::printCopy()
{
    Serial.println("Printing Copy ");
    for(int i=0;i<numCopySections;i++){
        CopyBlock section = copySections[i];
        int copyAddr = (int)(section.text);
        Serial.print(i);
        Serial.print(" ");
        Serial.print(section.x);
        Serial.print(",");
        Serial.print(section.y);
        Serial.print(" ");
        Serial.print(copyAddr);
        Serial.print(" ");
        Serial.println(section.text);
    }
}

SelectionArea &Page::getSelectionArea(int i)
{
    return this->choices[i];
}

int Page::getNumSelectionAreas()
{
    return numSelectionAreas;
}

void Page::addSelectionArea(int choiceIndex, int minX, int maxX, int minY, int maxY)
{
    // Serial.print("addSelectionArea ");
    // Serial.print(minX);
    // Serial.print(",");
    // Serial.print(minY);
    // Serial.print(",");
    // Serial.print(maxX);
    // Serial.print(",");
    // Serial.println(maxY);

    this->choices[numSelectionAreas] = {choiceIndex,minX,maxX,minY,maxY};
    numSelectionAreas++;
}

// void Page::addOption(char *option, int y, int maxY)
// {
//     this->copySections[this->numCopySections] = {option,y,maxY};
//     numCopySections++;
// }

CopyBlock Page::getCopy(int index)
{
    return this->copySections[index];
}

void Page::render(M5EPD_Canvas &canvas)
{
    canvas.clear();
    // Serial.println("Rendering");
    for(int i=0;i < this->numCopySections;i++){
        auto section = this->copySections[i];
        int lineAddr = *(section.text);
        // Serial.print(section.text);
        // Serial.print(" addr=");
        // Serial.print(lineAddr);
        // Serial.print(" x=");
        // Serial.print(section.x);
        // Serial.print(" y=");
        // Serial.print(section.y);
        // Serial.println(" ");
        canvas.drawString(this->copySections[i].text,this->copySections[i].x,this->copySections[i].y);
    }
}

Page &Paginator::currentPage()
{
    return *pages[currentPageIndex];
}

void Paginator::addCopy(const char *copy)
{
    if(numPages==0){
        this->addPage();
    }
    this->wordWrap(copy);
    //getLastPage()->addLine(copy,this->cursorY,);
}

void Paginator::addChoice(int choiceIndex, const char *copy)
{
    if(numPages==0){
        Serial.println("adding page");
        this->addPage();
    }
    this->cursorX = this->indent = 40;
    //Serial.println("word wrapping");
    int startPage = numPages-1;
    int startY = this->cursorY;
    int startX = this->cursorX;
    this->wordWrap(copy);
    //Serial.println("adding selection area");
    if(startPage < (numPages-1)){        
        for(int pageIndex=startPage;pageIndex<(numPages-1);pageIndex++){
            if(startY>=(m_canvas.height()-marginBottom)){
                startY=0;
                continue;
            }
            this->pages[pageIndex]->addSelectionArea(choiceIndex, startX,m_canvas.width(),startY,m_canvas.height()-marginBottom);
            startY = 0;
        }
    }
    this->getLastPage()->addSelectionArea(choiceIndex,startX,m_canvas.width()-marginRight,startY,cursorY);
    //currentPage().printCopy();
}

void Paginator::addLineBreak()
{
    if(numPages==0){
        this->addPage();
    }
    this->cursorY+=this->m_canvas.fontHeight();
    this->cursorX=this->indent;
    if(this->cursorY>this->m_canvas.height()){            
        this->addPage();            
    }
}

void Paginator::wordWrap(const char *text){
    char* alloc_text = (char*)malloc(strlen(text)+1);
    strcpy(alloc_text,text);
    char* line_c = alloc_text;
    
    while(line_c && *line_c!='\0'){
        // Serial.println("Settings out one line");
        //int16_t (*callbackMethod)(const char*) = &(M5EPD_Canvas::textWidth);
        //auto callbackMethod = std::mem_fn(static_cast<int16_t (M5EPD_Canvas::*)(const char*)>(&M5EPD_Canvas::textWidth));
        //auto callback = std::bind(static_cast<int16_t (M5EPD_Canvas::*)(const char*)>(&M5EPD_Canvas::textWidth), &this->m_canvas, std::placeholders::_1);
        M5EPD_Canvas &canvas = this->m_canvas;
        //std::function<int16_t(const char*)> callback = [&](const char* str) -> int16_t { return canvas.textWidth(str); };
        widthCallbackFunc callback = [&](const char* str) -> int16_t { return canvas.textWidth(str); };
        //auto callback = [&](const char* str) ->canvas.textWidth(str);
        
        // Serial.print("line pointer before wrap ");
        unsigned int lineAddr = (int)line_c;
        // Serial.print(lineAddr);
        // Serial.print(" ");
        // Serial.println(line_c);
        char *next_line = wrap_one_line(line_c, this->m_canvas.width()-cursorX-marginRight,callback);
        // Serial.print("line pointer after wrap ");
        lineAddr = (int)line_c;
        // Serial.print(lineAddr);
        // Serial.print(" ");
        // Serial.println(line_c);
        // Serial.print("Leftover:");
        // Serial.println(next_line);
        if(this->cursorY > (m_canvas.height()-m_canvas.fontHeight()-marginBottom)){
            Serial.println("Adding page");
            this->addPage();
        }
        this->getLastPage()->addLine(line_c, cursorX, cursorY);
        this->cursorY+=m_canvas.fontHeight();
        this->cursorX=this->indent;
        //next_line();
        
        line_c = next_line;
        // 
        // if(cursor_y>canvas.height()){
        //     break;
        // }
        
    }
    free(alloc_text);
}

Page* Paginator::getLastPage()
{
    if(this->numPages==0){
        Serial.println("Can't get last page, no pages added");
        return nullptr;
    }
    return pages[this->numPages-1];
}

Page* Paginator::addPage()
{
    if(numPages>this->MAX_PAGES){
        return nullptr;
    }
    pages[this->numPages] = new Page();
    this->numPages++;
    this->cursorX=indent;
    this->cursorY=marginTop;
    return this->getLastPage();
}


// void Paginator::addSelectableCopy(char *copy)
// {
    
// }

void Paginator::clear()
{
    for(int i=0;i<numPages;i++){
        delete(this->pages[i]);
    }
    numPages=0;
    indent=marginLeft;
    cursorX=indent;
    cursorY=marginTop;
    currentPageIndex=0;
}

bool Paginator::hasChoices()
{
    for(int i=0;i<this->numPages;i++){
        if(this->pages[i]->getNumSelectionAreas()>0){
            return true;
        }        
    }
    return false;
}

void Paginator::renderPage()
{
    Page &page = currentPage();
    page.render(this->m_canvas);
    m_canvas.pushCanvas(0, canvasOffsetY, UPDATE_MODE_DU4);
}

bool Paginator::nextPage()
{
    int nextIndex = this->currentPageIndex+1;
    if(nextIndex>=this->numPages){
        return false;
    }
    this->currentPageIndex = nextIndex;
    return true;
}

bool Paginator::previousPage()
{
    int prevIndex = this->currentPageIndex-1;
    if(prevIndex<=-1){
        return false;
    }
    this->currentPageIndex = prevIndex;
    return true;
}
