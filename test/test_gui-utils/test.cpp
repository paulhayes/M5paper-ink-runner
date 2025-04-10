#include "gui-utils.h"
#include <string.h>
#include <unity.h>
#include <Arduino.h>
//#include <cstring>
//#include <ostream>

int test_length(const char *str)
{
    return 10*strlen(str);
}

void test_wordwrap()
{
    char paragraph[] = "Mary had a little lamb it's fleece was white as snow. And everywhere that Mary went, the lamb was sure to go";
    const char *lines[] = {
        "Mary had a little",
        "lamb it's fleece was",
        "white as snow. And",
        "everywhere that Mary",
        "went, the lamb was",
        "sure to go"
    };

    char *current_line = paragraph;
    int max_line_width = 200;
    int numLines=0;
    
    char buf[64];
    //sprintf(buf,"Argh %d %d",(uint32_t)paragraph,(uint32_t)current_line);  
    //TEST_ASSERT_EQUAL_INT_MESSAGE(3,4,buf);

    while(current_line!=NULL && current_line[0]!='\0'){
        char *last_line = current_line;
        current_line = wrap_one_line(current_line, max_line_width, test_length);
        sprintf(buf,"Incorrect line %d %d %d %d",numLines,strlen(last_line),(uint32_t)current_line,(uint32_t)paragraph);
        if(numLines<6){
            TEST_ASSERT_EQUAL_STRING_MESSAGE(lines[numLines],last_line,buf);
        }
        numLines++;
        if(numLines>32){
            break;
        }
    }

    TEST_ASSERT_EQUAL(6,numLines);
}

void trivialTest(void) {
    TEST_ASSERT_EQUAL(4, 3+1);
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(trivialTest);
    RUN_TEST(test_wordwrap);
    UNITY_END();
}

void loop(){
    
}