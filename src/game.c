#include <stdio.h>
#include "game.h"
#include "board.h"

static void clear_input_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

void game_loop(void) {
    Board board;
    int player = 0;

    board_init(&board);

    while (!board_full(&board)) {
        int r1, c1, r2, c2;
        int result;

        board_print(&board);
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
            printf("Box completed. Player %c plays again.\n", player == 0 ? 'A' : 'B');
        }
    }

    board_print(&board);

    if (board_winner(&board) == 'T') {
        printf("Game over. It is a tie.\n");
    } else {
        printf("Game over. Player %c wins.\n", board_winner(&board));
    }
}
