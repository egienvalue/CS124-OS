#include "interrupts.h"
#include "timer.h"
#include "keyboard.h"
#include "video.h"
#include "game.h"

#include <stdint.h>

/* This is the entry-point for the game! */
void c_start(void) {
    /* TODO:  You will need to initialize various subsystems here.  This
     *        would include the interrupt handling mechanism, and the various
     *        systems that use interrupts.  Once this is done, you can call
     *        enable_interrupts() to start interrupt handling, and go on to
     *        do whatever else you decide to do!
     */

    init_interrupts();

    init_timer();
    init_keyboard();
    init_video();

    enable_interrupts();

    init_game_state();
    draw_game_start();

    game_loop();
}


typedef struct Space_Invaders {
    /* Info presented to user */
    uint8_t info_bar_height;
    uint32_t score; // number of enemies killed
    uint8_t lives_remaining;

    /* Position and direction of enemies */
    // top, left corner off enemy matrix. all are pixel units
    uint16_t enemy_mat_position_x; 
    uint16_t enemy_mat_position_y;
    uint16_t enemy_mat_width;
    uint16_t enemy_mat_height;

    uint16_t num_enemies_left;

    /* array of ints contraining num enemies per column (assume no 
    gaps between enemies in a column, so it represents a matrix) */
    uint8_t num_enemy_cols;
    uint8_t num_enemies_per_col[NUM_ENEMY_COLS];

    // user position
    uint8_t user_bar_height;
    uint8_t user_position_x;
    uint8_t user_position_y; // always the lowest row (not dynamic)

} Space_Invaders;

static Space_Invaders game;


void init_game_state(void) {
    game.score = 0;
    game.lives_remaining = NUM_LIVES;

    game.info_bar_height = VID_HEIGHT * INFO_BAR_HEIGHT;
    game.user_bar_height = USER_SIZE;
    game.user_position_y = VID_HEIGHT - game.user_bar_height;
    // user starts off in middle of screen
    game.user_position_x = (VID_WIDTH - USER_SIZE) / 2;

    // enemies start of in aligned center, right below the info space
    // height enemy mat will use
    game.enemy_mat_height = ((VID_HEIGHT - game.info_bar_height 
        - game.user_bar_height) * ENEMY_MAT_HEIGHT);
    game.enemy_mat_width = VID_WIDTH * ENEMY_MAT_WIDTH;
    // enemy starts off at center of screen
    game.enemy_mat_position_x = ((VID_WIDTH - game.enemy_mat_width) / 2);
    game.enemy_mat_position_y = game.user_bar_height; // start at top

    // set maximum # enemies per col to each col's num_enemies_per_col
    uint8_t max_enemies_per_col = (game.enemy_mat_height / 
        (ENEMY_SIZE + ENEMY_SPACING));
    game.num_enemy_cols = ((VID_WIDTH * ENEMY_MAT_WIDTH) / 
        (ENEMY_SIZE + ENEMY_SPACING));

    for (int c = 0; c < game.num_enemy_cols; c++) {
        game.num_enemies_per_col[c] = max_enemies_per_col;
    }

    game.num_enemies_left = (max_enemies_per_col * game.num_enemy_cols);
}


void draw_game_start(void) {
    // draw info bar
    draw_box(0, 0, VID_WIDTH, game.info_bar_height, 1); // blue

    // // draw a test figure and movie it around
    // draw_box(VID_WIDTH / 2, VID_HEIGHT / 2, 10, 10, 2);
    // sleep(1.0);
    // draw_box(VID_WIDTH / 2, VID_HEIGHT / 2, 10, 10, 3);
    // sleep(1.0);
    // draw_box(VID_WIDTH / 2, VID_HEIGHT / 2, 10, 10, 4);

    // draw user in user bar
    draw_box(game.user_position_x, game.user_position_y, USER_SIZE, USER_SIZE, 14); // yellow

    // draw enemies
    int num_enemies_in_col, ex, ey;
    ex = game.enemy_mat_position_x;

    for (int c = 0; c < game.num_enemy_cols; c++) {
        num_enemies_in_col = game.num_enemies_per_col[c];
        ey = game.enemy_mat_position_y;

        for (int e = 0; e < num_enemies_in_col; e++) {
            draw_box(ex, ey, ENEMY_SIZE, ENEMY_SIZE, 2);
            ey += ENEMY_SIZE + ENEMY_SPACING;
            // set collision detection with user here
        }
        
        ex += ENEMY_SIZE + ENEMY_SPACING;
    }

}


void move_user(int dx) {
    // move user
    game.user_position_x += dx;

    // redraw user
    draw_box(0, game.user_position_y, VID_WIDTH, game.user_bar_height, 0);
    draw_box(game.user_position_x, game.user_position_y, USER_SIZE, USER_SIZE, 14); // yellow
}


void game_loop(void) {
    unsigned char keycode;
    char empty = KEY_QUEUE_EMPTY;
    uint8_t color = 0;
    
    while (1) {
        keycode = key_queue_pop();
        if (keycode != KEY_QUEUE_EMPTY) {
            if (keycode == LEFT_ARROW) {
                move_user(-1);
            } else if (keycode == RIGHT_ARROW) {
                move_user(1);
            }
            // color++;
            // draw_box(VID_WIDTH / 2, VID_HEIGHT / 2, 10, 10, color);
        }
        // sleep(.1);
    }
}
