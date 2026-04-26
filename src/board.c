#include <stdio.h>
#include <stdlib.h>

#include "board.h"

static int in_range_dot(int row, int col) {
    return row >= 0 && row < DOT_ROWS && col >= 0 && col < DOT_COLS;
}

static int claim_box(Board *board, int row, int col, int player) {
    if (row < 0 || row >= BOX_ROWS || col < 0 || col >= BOX_COLS) {
        return 0;
    }

    if (board->owner[row][col] != '\0') {
        return 0;
    }

    if (board->h[row][col] && board->h[row + 1][col]
        && board->v[row][col] && board->v[row][col + 1]) {
        board->owner[row][col] = player == 0 ? 'A' : 'B';
        if (player == 0) {
            board->scoreA++;
        } else {
            board->scoreB++;
        }
        return 1;
    }

    return 0;
}

void board_init(Board *board) {
    int row;
    int col;

    for (row = 0; row < DOT_ROWS; row++) {
        for (col = 0; col < BOX_COLS; col++) {
            board->h[row][col] = 0;
        }
    }

    for (row = 0; row < BOX_ROWS; row++) {
        for (col = 0; col < DOT_COLS; col++) {
            board->v[row][col] = 0;
        }
    }

    for (row = 0; row < BOX_ROWS; row++) {
        for (col = 0; col < BOX_COLS; col++) {
            board->owner[row][col] = '\0';
        }
    }

    board->scoreA = 0;
    board->scoreB = 0;
}

void board_print(const Board *board) {
    int row;
    int col;

    printf("\n  ");
    for (col = 0; col < DOT_COLS; col++) {
        printf("%d ", col);
    }
    printf("\n");

    for (row = 0; row < DOT_ROWS; row++) {
        printf("%d ", row);
        for (col = 0; col < DOT_COLS; col++) {
            printf(".");
            if (col < BOX_COLS) {
                printf("%c", board->h[row][col] ? '-' : ' ');
            }
        }
        printf("\n");

        if (row < BOX_ROWS) {
            printf("  ");
            for (col = 0; col < DOT_COLS; col++) {
                printf("%c", board->v[row][col] ? '|' : ' ');
                if (col < BOX_COLS) {
                    printf("%c", board->owner[row][col] ? board->owner[row][col] : ' ');
                }
            }
            printf("\n");
        }
    }

    printf("\nPlayer A score: %d\n", board->scoreA);
    printf("Player B score: %d\n\n", board->scoreB);
}

int board_apply_move(Board *board, int player, int r1, int c1, int r2, int c2) {
    int claimed = 0;

    if (!in_range_dot(r1, c1) || !in_range_dot(r2, c2)) {
        return -1;
    }

    if (abs(r1 - r2) + abs(c1 - c2) != 1) {
        return -1;
    }

    if (r1 == r2) {
        int row = r1;
        int col = c1 < c2 ? c1 : c2;

        if (board->h[row][col]) {
            return -1;
        }

        board->h[row][col] = 1;
        if (row > 0) {
            claimed += claim_box(board, row - 1, col, player);
        }
        if (row < BOX_ROWS) {
            claimed += claim_box(board, row, col, player);
        }
    } else {
        int row = r1 < r2 ? r1 : r2;
        int col = c1;

        if (board->v[row][col]) {
            return -1;
        }

        board->v[row][col] = 1;
        if (col > 0) {
            claimed += claim_box(board, row, col - 1, player);
        }
        if (col < BOX_COLS) {
            claimed += claim_box(board, row, col, player);
        }
    }

    return claimed;
}

int board_full(const Board *board) {
    int row;
    int col;

    for (row = 0; row < BOX_ROWS; row++) {
        for (col = 0; col < BOX_COLS; col++) {
            if (board->owner[row][col] == '\0') {
                return 0;
            }
        }
    }

    return 1;
}

char board_winner(const Board *board) {
    if (board->scoreA > board->scoreB) {
        return 'A';
    }
    if (board->scoreB > board->scoreA) {
        return 'B';
    }
    return 'T';
}
