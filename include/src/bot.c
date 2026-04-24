#include "bot.h"

static int in_range_dot(int r, int c) {
    return r >= 0 && r < DOT_ROWS && c >= 0 && c < DOT_COLS;
}

static int is_valid_move(const Board *b, int r1, int c1, int r2, int c2) {
    if (!in_range_dot(r1, c1) || !in_range_dot(r2, c2)) return 0;
    if ((r1 - r2) * (r1 - r2) + (c1 - c2) * (c1 - c2) != 1) return 0;

    if (r1 == r2) {
        int rr = r1;
        int cc = (c1 < c2) ? c1 : c2;
        return b->h[rr][cc] == 0;
    } else {
        int rr = (r1 < r2) ? r1 : r2;
        int cc = c1;
        return b->v[rr][cc] == 0;
    }
}

static int move_is_horizontal(int r1, int c1, int r2, int c2) {
    return r1 == r2;
}

static int box_sides_after_move(const Board *b, int br, int bc,
                                int r1, int c1, int r2, int c2) {
    int top = b->h[br][bc];
    int bottom = b->h[br + 1][bc];
    int left = b->v[br][bc];
    int right = b->v[br][bc + 1];

    if (move_is_horizontal(r1, c1, r2, c2)) {
        int rr = r1;
        int cc = (c1 < c2) ? c1 : c2;
        if (rr == br && cc == bc) top = 1;
        if (rr == br + 1 && cc == bc) bottom = 1;
    } else {
        int rr = (r1 < r2) ? r1 : r2;
        int cc = c1;
        if (rr == br && cc == bc) left = 1;
        if (rr == br && cc == bc + 1) right = 1;
    }

    return top + bottom + left + right;
}

static int count_completed_boxes(const Board *b, int r1, int c1, int r2, int c2) {
    int count = 0;

    if (move_is_horizontal(r1, c1, r2, c2)) {
        int rr = r1;
        int cc = (c1 < c2) ? c1 : c2;

        if (rr > 0 && b->owner[rr - 1][cc] == '\0' &&
            box_sides_after_move(b, rr - 1, cc, r1, c1, r2, c2) == 4) {
            count++;
        }
        if (rr < BOX_ROWS && b->owner[rr][cc] == '\0' &&
            box_sides_after_move(b, rr, cc, r1, c1, r2, c2) == 4) {
            count++;
        }
    } else {
        int rr = (r1 < r2) ? r1 : r2;
        int cc = c1;

        if (cc > 0 && b->owner[rr][cc - 1] == '\0' &&
            box_sides_after_move(b, rr, cc - 1, r1, c1, r2, c2) == 4) {
            count++;
        }
        if (cc < BOX_COLS && b->owner[rr][cc] == '\0' &&
            box_sides_after_move(b, rr, cc, r1, c1, r2, c2) == 4) {
            count++;
        }
    }

    return count;
}

static int count_third_sides_created(const Board *b, int r1, int c1, int r2, int c2) {
    int count = 0;

    if (move_is_horizontal(r1, c1, r2, c2)) {
        int rr = r1;
        int cc = (c1 < c2) ? c1 : c2;

        if (rr > 0 && b->owner[rr - 1][cc] == '\0' &&
            box_sides_after_move(b, rr - 1, cc, r1, c1, r2, c2) == 3) {
            count++;
        }
        if (rr < BOX_ROWS && b->owner[rr][cc] == '\0' &&
            box_sides_after_move(b, rr, cc, r1, c1, r2, c2) == 3) {
            count++;
        }
    } else {
        int rr = (r1 < r2) ? r1 : r2;
        int cc = c1;

        if (cc > 0 && b->owner[rr][cc - 1] == '\0' &&
            box_sides_after_move(b, rr, cc - 1, r1, c1, r2, c2) == 3) {
            count++;
        }
        if (cc < BOX_COLS && b->owner[rr][cc] == '\0' &&
            box_sides_after_move(b, rr, cc, r1, c1, r2, c2) == 3) {
            count++;
        }
    }

    return count;
}

Move choose_bot_move(const Board *b) {
    Move best_finish = { -1, -1, -1, -1 };
    Move best_safe = { -1, -1, -1, -1 };
    Move best_risk = { -1, -1, -1, -1 };
    int best_finish_score = -1;
    int best_risk_score = 1000000;
    int r, c;

    for (r = 0; r < DOT_ROWS; r++) {
        for (c = 0; c < DOT_COLS - 1; c++) {
            int done, risk;
            if (!is_valid_move(b, r, c, r, c + 1)) continue;

            done = count_completed_boxes(b, r, c, r, c + 1);
            if (done > best_finish_score) {
                best_finish_score = done;
                best_finish = (Move){r, c, r, c + 1};
            }

            risk = count_third_sides_created(b, r, c, r, c + 1);
            if (risk == 0 && best_safe.r1 == -1) {
                best_safe = (Move){r, c, r, c + 1};
            }
            if (risk < best_risk_score) {
                best_risk_score = risk;
                best_risk = (Move){r, c, r, c + 1};
            }
        }
    }

    for (r = 0; r < DOT_ROWS - 1; r++) {
        for (c = 0; c < DOT_COLS; c++) {
            int done, risk;
            if (!is_valid_move(b, r, c, r + 1, c)) continue;

            done = count_completed_boxes(b, r, c, r + 1, c);
            if (done > best_finish_score) {
                best_finish_score = done;
                best_finish = (Move){r, c, r + 1, c};
            }

            risk = count_third_sides_created(b, r, c, r + 1, c);
            if (risk == 0 && best_safe.r1 == -1) {
                best_safe = (Move){r, c, r + 1, c};
            }
            if (risk < best_risk_score) {
                best_risk_score = risk;
                best_risk = (Move){r, c, r + 1, c};
            }
        }
    }

    if (best_finish_score > 0) return best_finish;
    if (best_safe.r1 != -1) return best_safe;
    return best_risk;
}
