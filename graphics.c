/** @file   graphics.c
    @author Gavin Ong, Enyang Zhang
    @date   19 October 2017
    @brief  Display functions for Flappybird game.
*/

#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "pio.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "graphics.h"
#include "pipe.h"

/**Display given pipe on the LED matrix
 * @param pipe the pipe containing hole position. */
void display_pipe(Pipe pipe)
{
    tinygl_clear();
    int count = 0;
    while (count < 7) {
        if(count != pipe.hole_location && count != pipe.hole_location + 1 ) {
            tinygl_draw_point (tinygl_point (pipe.current_col, count), 1);
        }
        count += 1;
    }
}

/** Display bird on the LED matrix. */
void display_bird(int bird_col, int bird_row)
{
    tinygl_clear();
    tinygl_draw_point (tinygl_point (bird_col, bird_row), 1);
}


