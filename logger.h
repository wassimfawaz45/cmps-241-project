#ifndef LOGGER_H
#define LOGGER_H

#include <pthread.h>
#include <time.h>

#include "board.h"

typedef struct {
    pthread_t thread;
    pthread_mutex_t mutex;
    int running;
    int moves_played;
    int last_player;
    time_t started_at;
    Board snapshot;
    char log_path[256];
} MatchLogger;

int logger_start(MatchLogger *logger, const char *log_path, const Board *initial_board);
void logger_record_move(MatchLogger *logger, const Board *board, int player);
void logger_stop(MatchLogger *logger);

#endif
