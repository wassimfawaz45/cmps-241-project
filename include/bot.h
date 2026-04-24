#ifndef BOT_H
#define BOT_H

#include "board.h"

typedef struct {
    int r1, c1, r2, c2;
} Move;

Move choose_bot_move(const Board *b);

#endif
