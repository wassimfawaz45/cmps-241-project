#include <stdio.h>
#include <stdlib.h>
#include "board.h"

static int in_range_dot(int r, int c) {
    return r >= 0 && r < DOT_ROWS && c >= 0 && c < DOT_COLS;
}

static int claim_box(Board *b, int r, int c, int player) {
    if (r < 0 || r >= BOX_ROWS || c < 0 || c >= BOX_COLS) return 0;
    if (b->owner[r][c] != '\0') return 0;

    if (b->h[r][c] && b->h[r + 1][c] && b->v[r][c] && b->v[r][c + 1]) {
        b->owner[r][c] = player == 0 ? 'A' : 'B';
        if (player == 0) b->scoreA++;
        else b->scoreB++;
        return 1;
    }
    return 0;
}

void board_init(Board *b) {
    int r, c;
    for (r = 0; r < DOT_ROWS; r++) {
        for (c = 0; c < BOX_COLS; c++) b->h[r][c] = 0;
    }
    for (r = 0; r < BOX_ROWS; r++) {
        for (c = 0; c < DOT_COLS; c++) b->v[r][c] = 0;
    }
    for (r = 0; r < BOX_ROWS; r++) {
        for (c = 0; c < BOX_COLS; c++) b->owner[r][c] = '\0';
    }
    b->scoreA = 0;
    b->scoreB = 0;
}

void board_print(const Board *b) {
    int r, c;

    printf("\n   ");
    for (c = 0; c < DOT_COLS; c++) printf("%d ", c);
    printf("\n");

    for (r = 0; r < DOT_ROWS; r++) {
        printf("%d  ", r);
        for (c = 0; c < DOT_COLS; c++) {
            printf(".");
            if (c < BOX_COLS) printf(b->h[r][c] ? "-" : " ");
        }
        printf("\n");

        if (r < BOX_ROWS) {
            printf("   ");
            for (c = 0; c < DOT_COLS; c++) {
                printf("%c", b->v[r][c] ? '|' : ' ');
                if (c < BOX_COLS) {
                    char ch = b->owner[r][c] ? b->owner[r][c] : ' ';
                    printf("%c", ch);
                }
            }
            printf("\n");
        }
    }

    printf("\nPlayer A score: %d\n", b->scoreA);
    printf("Player B score: %d\n\n", b->scoreB);
}

int board_apply_move(Board *b, int player, int r1, int c1, int r2, int c2) {
    int claimed = 0;

    if (!in_range_dot(r1, c1) || !in_range_dot(r2, c2)) return -1;
    if (abs(r1 - r2) + abs(c1 - c2) != 1) return -1;

    if (r1 == r2) {
        int rr = r1;
        int cc = c1 < c2 ? c1 : c2;
        if (b->h[rr][cc]) return -1;
        b->h[rr][cc] = 1;

        if (rr > 0) claimed += claim_box(b, rr - 1, cc, player);
        if (rr < BOX_ROWS) claimed += claim_box(b, rr, cc, player);
    } else {
        int rr = r1 < r2 ? r1 : r2;
        int cc = c1;
        if (b->v[rr][cc]) return -1;
        b->v[rr][cc] = 1;

        if (cc > 0) claimed += claim_box(b, rr, cc - 1, player);
        if (cc < BOX_COLS) claimed += claim_box(b, rr, cc, player);
    }

    return claimed;
}

int board_full(const Board *b) {
    int r, c;
    for (r = 0; r < BOX_ROWS; r++) {
        for (c = 0; c < BOX_COLS; c++) {
            if (b->owner[r][c] == '\0') return 0;
        }
    }
    return 1;
}

char board_winner(const Board *b) {
    if (b->scoreA > b->scoreB) return 'A';
    if (b->scoreB > b->scoreA) return 'B';
    return 'T';
}
