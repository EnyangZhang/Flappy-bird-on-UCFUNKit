/** @file   graphics.h
    @author Gavin Ong, Enyang Zhang
    @date   19 September 2017
    @brief  Display functions for flappy bird game
*/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "system.h"
#include "pipe.h"

void display_pipe(Pipe pipe);

void display_bird(int col, int row);


#endif

