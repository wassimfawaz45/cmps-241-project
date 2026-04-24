#include <stdio.h>
#include "game.h"
#include "board.h"
#include "bot.h"

static void clear_input_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

static int choose_mode(void) {
    int mode;
    while (1) {
        printf("Choose mode:\n");
        printf("1. Player vs Player\n");
        printf("2. Player vs Bot\n");
        printf("Enter choice: ");

        if (scanf("%d", &mode) != 1) {
            printf("Invalid input.\n");
            clear_input_line();
            continue;
        }

        if (mode == 1 || mode == 2) {
            return mode;
        }

        printf("Please choose 1 or 2.\n");
    }
}

void game_loop(void) {
    Board board;
    int player = 0;
    int mode = choose_mode();

    board_init(&board);

    while (!board_full(&board)) {
        int result;

        board_print(&board);

        if (mode == 2 && player == 1) {
            Move m = choose_bot_move(&board);
            printf("Bot B plays: %d %d %d %d\n", m.r1, m.c1, m.r2, m.c2);
            result = board_apply_move(&board, player, m.r1, m.c1, m.r2, m.c2);

            if (result < 0) {
                printf("Bot made an invalid move. Stopping.\n");
                return;
            }

            if (result == 0) {
                player = 0;
            } else {
                printf("Bot B completed a box and plays again.\n");
            }
        } else {
            int r1, c1, r2, c2;

            printf("Player %c turn. Enter r1 c1 r2 c2: ", player == 0 ? 'A' : 'B');

            if (scanf("%d %d %d %d", &r1, &c1, &r2, &c2) != 4) {
                printf("Invalid input. Please enter 4 integers.\n");
                clear_input_line();
                continue;
            }

            result = board_apply_move(&board, player, r1, c1, r2, c2);

            if (result < 0) {
                printf("Invalid move. Try again.\n");
                continue;
            }

            if (result == 0) {
                player = 1 - player;
            } else {
                printf("Player %c completed a box and plays again.\n", player == 0 ? 'A' : 'B');
            }
        }
    }

    board_print(&board);

    if (board_winner(&board) == 'T') {
        printf("Game over. It is a tie.\n");
    } else {
        printf("Game over. Player %c wins.\n", board_winner(&board));
    }
}
