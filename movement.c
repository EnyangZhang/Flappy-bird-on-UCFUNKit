/** @file   movement.c
    @author Gavin Ong, Enyang Zhang
    @date   19 October 2017
    @brief  Movement functions for Flappybird game.
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
#include "pipe.h"
#include "global.h"

#define FREQUENCY 5000
#define FALL_FREQ_ONE (FREQUENCY / 10 * 4)
#define FALL_FREQ_TWO (FALL_FREQ_ONE / 5 * 3)
#define FALL_FREQ_THREE (FALL_FREQ_TWO / 3 * 2)
#define FALL_FREQ_FOUR (FALL_FREQ_TWO / 8 * 5)

/** Make pipe move and generate random hole location for each pipe
    @param pipe pointer to the pipe to be moved */
void move_pipe(Pipe* pipe)
{
    if(pipe->display == true) {
        pipe->current_col -= 1;
        if(pipe->current_col < -1) {
            pipe->current_col = 10;
            int rand_int = 0;
            rand_int = rand() % 5;
            pipe->hole_location = rand_int;
        }
    }
}

/** Moves the bird up and down */
void move_bird(void)
{
    if(pressed == false) {
        bird_row += 1;
        if(ticks >= (FALL_FREQ_ONE)) { //Make the bird fall faster
            ticks = (FALL_FREQ_TWO);
        } else if(ticks == FALL_FREQ_TWO) {
            ticks = FALL_FREQ_THREE;
        } else if(ticks == FALL_FREQ_THREE) {
            ticks = FALL_FREQ_FOUR;
        }
    } else {
        if (bird_row >= 1) {
            bird_row -= 1;
        }
        up_count -= 1;
        if(ticks == FALL_FREQ_FOUR) { //Make the bird go up slower
            ticks = FALL_FREQ_THREE;
        } else if(ticks == FALL_FREQ_THREE) {
            ticks = FALL_FREQ_TWO;
        } else if(ticks == FALL_FREQ_TWO) {
            ticks = FALL_FREQ_ONE;
        }
        if (up_count == 0) {
            pressed = false;
            ticks = FALL_FREQ_ONE;
        }
    }
    ticks_before_moving = ticks;
}
