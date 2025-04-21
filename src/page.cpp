#include <functional>
#include <cstring>
#include "page.hpp"
#include "gui-utils.h"

void Page::addLine(const char *copy, int x, int y)
{    
    copySections[numCopySections] = {copy,x,y};
    numCopySections++;
}

SelectionArea &Page::getChoice(int i)
{
    return this->choices[i];
}

void Page::addSelectionArea(int choiceIndex, int minX, int maxX, int minY, int maxY)
{
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

void Page::render(M5EPD_Canvas canvas)
{
    canvas.clear();
    for(int i=0;i < this->numCopySections;i++){
        canvas.drawString(this->copySections[i].text,this->copySections[i].x,this->copySections[i].y);
    }
}

Page &Paginator::currentPage()
{
    return *pages[currentPageIndex];
}

void Paginator::addCopy(char *copy)
{
    if(numPages==0){
        this->addPage();
    }
    this->wordWrap(copy);
    //getLastPage()->addLine(copy,this->cursorY,);
}

void Paginator::addChoice(int choiceIndex, char *copy)
{
    if(numPages==0){
        this->addPage();
    }
    int startPage = this->currentPageIndex;
    int startY = this->cursorY;
    int startX = this->cursorX;
    this->wordWrap(copy);
    if(startPage < this->currentPageIndex){        
        for(int pageIndex=startPage;pageIndex<this->currentPageIndex;pageIndex++){
            this->pages[pageIndex]->addSelectionArea(choiceIndex, startX,this->m_canvas.width(),startY,this->m_canvas.height());
            startY = 0;
        }
    }
    this->currentPage().addSelectionArea(choiceIndex,startX,m_canvas.width(),startY,this->cursorY);
}

void Paginator::addLineBreak()
{
    if(numPages==0){
        this->addPage();
    }
    this->cursorY+=this->m_canvas.fontHeight();
    if(this->cursorY>this->m_canvas.height()){            
        this->addPage();            
    }
}

const char* Paginator::wordWrap(const char *line_c){
    while(line_c && *line_c!='\0'){
        auto callback = std::bind(&M5EPD_Canvas::textWidth, &this->m_canvas, std::placeholders::_1);
        const char *next_line = wrap_one_line(this->m_canvas, line_c, this->m_canvas.width(),callback);
        //next_line();
        if(this->cursorY>this->m_canvas.height()){
            
            this->addPage();            
        }
        this->getLastPage()->addLine(next_line, this->cursorX, this->cursorX);
        next_line = line_c;
        // 
        // if(cursor_y>canvas.height()){
        //     break;
        // }
        
    }
    return line_c;
}

Page* Paginator::getLastPage()
{
    return pages[this->numPages-1];
}

Page* Paginator::addPage()
{
    if(numPages>this->MAX_PAGES){
        return nullptr;
    }
    pages[this->numPages] = new Page();
    this->numPages++;
    this->cursorX=0;
    this->cursorY=0;
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
    this->numPages=0;
    cursorX=0;
    cursorY=0;
}

bool Paginator::hasChoices()
{
    for(int i=0;i<this->numPages;i++){
        if(this->pages[i]->numOptions>0){
            return true;
        }        
    }
    return false;
}

void Paginator::renderPage()
{
    Page &page = currentPage();
    page.render(this->m_canvas);
    
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
