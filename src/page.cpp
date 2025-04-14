#include "page.hpp"
#include "gui-utils.h"
#include <cstring>

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

Page &Paginator::currentPage()
{
    return *pages[currentPageIndex];
}

void Paginator::addCopy(char *copy)
{
    if(numPages==0){
        this->addPage();
    }
    this->wordWrap()
    //getLastPage()->addLine(copy,this->cursorY,);
}

void Paginator::addChoice(char *copy)
{
    
}

const char* Paginator::wordWrap(const char *line_c){
    while(line_c && *line_c!='\0'){
        const char *next_line = wrap_one_line(line_c);
        //next_line();
        if(this->cursorY>this->m_canvas.height()){
            gui_clear();
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
    return this->getLastPage();
}


void Paginator::addSelectableCopy(char *copy)
{

}

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
