#include "page.hpp"

void Page::addCopy(char *copy,int length)
{
    copySections[numCopySections] = copy;
    numCopySections++;
}

void Page::addOption(char *option, int minY, int maxY)
{
    options[numOptions] = {option,minY,maxY};
    numOptions++;
}