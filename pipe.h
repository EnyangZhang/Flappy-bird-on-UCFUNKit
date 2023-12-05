/** @file   pipe.h
    @author Gavin Ong, Enyang Zhang
    @date   19 September 2017
    @brief  Pipe structure for Flappy bird game
*/

#ifndef PIPE_H
#define PIPE_H

typedef struct pipes_s {
    int hole_location;
    int current_col;
    bool display;
} Pipe;


#endif

