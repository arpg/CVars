/*******************************************************************
 *
 *  CVars demo using standard input.
 *
 *******************************************************************/

#include <stdio.h>

#include <TextConsole/TextConsole.h>


int main( int, char** ) 
{
    TextConsole Console;

    while(1){
        int c = KbHit();
        Console.ProcessKey( c );
    }
}

