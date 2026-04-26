#ifndef BOARD_H
#define BOARD_H

#define BOX_ROWS 4
#define BOX_COLS 5
#define DOT_ROWS (BOX_ROWS + 1)
#define DOT_COLS (BOX_COLS + 1)

typedef struct {
    int h[DOT_ROWS][BOX_COLS];
    int v[BOX_ROWS][DOT_COLS];
    char owner[BOX_ROWS][BOX_COLS];
    int scoreA;
    int scoreB;
} Board;

void board_init(Board *board);
void board_print(const Board *board);
int board_apply_move(Board *board, int player, int r1, int c1, int r2, int c2);
int board_full(const Board *board);
char board_winner(const Board *board);

#endif
