#include <SDL.h>

#include "simple_logger.h"
#include "director.h"

int main(int argc, char* argv[])
{

    director_init_game();

    /*main game loop*/
    while (!director_run_game())
    {
    }
    director_end_game();
    return 0;
}


/*eol@eof*/