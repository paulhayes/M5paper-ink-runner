#ifndef __INK_PLAYER_H
#define __INK_PLAYER_H

void write_story_section(Paginator &paginator);
void write_choices(Paginator &paginator);
void contine_story(Paginator &paginator);
void story_choice(int choice);
void load_story(const char* story_filename);
#endif