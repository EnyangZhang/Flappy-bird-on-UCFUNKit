/*
 * ENCE260 game project
 * Group members: Gavin Ong (Student ID: 18376463), Enyang Zhang (Student ID: 72004622)
 * A flappy bird like game
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
#include "global.h"

#define PACER_RATE 5000
#define MESSAGE_RATE 20
#define FREQUENCY 5000
#define TICKS_BEFORE_MOVE_PIPE (FREQUENCY / 2)
#define TICKS_BEFORE_MOVING (FREQUENCY / 10 * 4)
#define TICKS_BEFORE_PIPE_TWO (TICKS_BEFORE_MOVE_PIPE * 3)
#define TICKS_BEFORE_PIPE_THREE (TICKS_BEFORE_MOVE_PIPE * 6)
#define TICKS_BEFORE_PIPE_FOUR (TICKS_BEFORE_MOVE_PIPE * 9)
#define START 0
#define GAME 1
#define GAME_OVER 2
#define PLAYER_ONE 1
#define PLAYER_TWO 2
#define GAME_START_SIGNAL 'B'
#define GAME_OVER_SIGNAL 'H'
#define GAME_RESET_SIGNAL 'Z'


int bird_row; //bird y
int bird_col; //bird x
int ticks = TICKS_BEFORE_MOVING;
int ticks_before_moving = TICKS_BEFORE_MOVING;
int up_count = 1;
int pipe_ticks = TICKS_BEFORE_MOVE_PIPE;
int nav_ticks = 0;
int total_ticks = 0;
int pipe_no = 0;
int state = START;
int player = 0;
bool pressed = false;
Pipe pipe1 = {5 ,6, true};
Pipe pipe2 = {3, 6, false};
Pipe pipe3 = {1, 6, false};
Pipe pipe4 = {4, 6, false};

/** Checks if bird collides with given pipe
    @param pipe to check if the bird collides with*/
void check_collision(Pipe pipe)
{
    if(pipe.current_col == 0 && bird_row != pipe.hole_location && bird_row != (pipe.hole_location + 1)) {
        state = GAME_OVER;
        tinygl_clear();
        ir_uart_putc(GAME_OVER_SIGNAL);
        tinygl_text("LOSER");
        pacer_wait();
    }
}

/** Check if bird hits pipes, then end game if it hits. */
void pipes_hitting_check(void)
{
    check_collision(pipe1);
    check_collision(pipe2);
    check_collision(pipe3);
    check_collision(pipe4);
}

/** Update the location of all pipes. */
void update_pipes(void)
{
    move_pipe(&pipe1);
    move_pipe(&pipe2);
    move_pipe(&pipe3);
    move_pipe(&pipe4);
}

/** Alternates display between pipes and bird */
void display(void)
{
    if (pipe_no == 0 || pipe_no == 2 || pipe_no == 4 || pipe_no == 6) {
        display_bird(bird_col, bird_row);
        pipe_no += 1;
    } else if(pipe_no == 1) {
        display_pipe(pipe1);
        pipe_no += 1;
    } else if(pipe_no == 3 && pipe2.display == true) {
        display_pipe(pipe2);
        pipe_no += 1;
    } else if(pipe_no == 5 && pipe3.display == true) {
        display_pipe(pipe3);
        pipe_no += 1;
    } else if(pipe_no == 7 && pipe4.display == true) {
        display_pipe(pipe4);
        pipe_no = 0;
    } else {
        pipe_no = 0;
    }
}

/** Check if the pipe should be displayed for the first time.
 *  @param ticks ticks until pipe is displayed for the first time. */
void check_display(int ticks)
{
    if(ticks >= TICKS_BEFORE_PIPE_TWO) {
        pipe2.display = true;
    }
    if(ticks >= TICKS_BEFORE_PIPE_THREE) {
        pipe3.display = true;
    }
    if(ticks >= TICKS_BEFORE_PIPE_FOUR) {
        pipe4.display = true;
    }
}

/** Checks if signal is received and change game state depending on signal.*/
void check_ir(void)
{
    if(ir_uart_read_ready_p()) {
        char received_character = ir_uart_getc();
        if(received_character == GAME_START_SIGNAL && player != PLAYER_ONE) {
            player = PLAYER_TWO;
            state = GAME;
        } else if(received_character == GAME_OVER_SIGNAL) {
            state = GAME_OVER;
            tinygl_clear();
            tinygl_text("WINNER");
            pacer_wait();

        } else if(received_character == GAME_RESET_SIGNAL) {
            state = START;
            tinygl_clear();
            tinygl_text("FLAPPYBIRD");
            bird_row = 0;
            bird_col = 0;
            player = 0;
            pipe1.current_col = 6;
            pipe1.display = true;
            pipe2.current_col = 6;
            pipe2.display = false;
            pipe3.current_col = 6;
            pipe3.display = false;
            pipe4.current_col = 6;
            pipe4.display = false;
            total_ticks = 0;
        }
    }
}

/** Checks if navswitch is pushed and changes game state or makes bird fly*/
void check_navswitch (void)
{
    if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
        if(state == START) { //If at title screen start game.
            state = GAME;
            tinygl_clear();
            player = PLAYER_ONE;
            ir_uart_putc(GAME_START_SIGNAL);
            pacer_wait ();
        }
        if(state == GAME) { //If in-game make bird fly
            pressed = true;
            up_count = 1;
            ticks = 1;
        }
        if(state == GAME_OVER) { //If game over reset and go back to title
            state = START;
            tinygl_clear();
            tinygl_text("FLAPPYBIRD");
            bird_row = 0;
            bird_col = 0;
            ir_uart_putc(GAME_RESET_SIGNAL);
            player = 0;
            pipe1.current_col = 6;
            pipe1.display = true;
            pipe2.current_col = 6;
            pipe2.display = false;
            pipe3.current_col = 6;
            pipe3.display = false;
            pipe4.current_col = 6;
            pipe4.display = false;
            total_ticks = 0;
            pacer_wait();
        }
    }
}

int main (void)
{
    system_init ();

    navswitch_init ();
    ir_uart_init();
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    bird_row = 0;
    bird_col = 0;
    display_bird(bird_col, bird_row);
    display_pipe(pipe1);
    pacer_init (PACER_RATE);
    tinygl_text("FLAPPYBIRD"); //Title screen
    /* Paced loop.  */
    while (1) {
        pacer_wait ();
        tinygl_update ();
        nav_ticks += 1;
        if(nav_ticks % 10 == 0) { //Check nav switch
            navswitch_update();
            check_navswitch();
            nav_ticks = 0;
        }
        check_ir();
        if(state == GAME) {
            pipe_ticks -= 1;
            if (pipe_ticks == 0) { //Move pipes
                update_pipes();
                pipe_ticks = TICKS_BEFORE_MOVE_PIPE;
            }
            display(); //Update display
            ticks_before_moving -= 1;
            if(ticks_before_moving <= 0) { //Move bird
                move_bird();
            }
            total_ticks +=1;
            check_display(total_ticks); //Display new pipes
            if (bird_row > 6) {
                state = GAME_OVER;
                tinygl_clear();
                ir_uart_putc('H');
                tinygl_text("LOSER");
                pacer_wait();
            }
            pipes_hitting_check();
        }

    }
}

