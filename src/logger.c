#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "logger.h"

static void append_snapshot(const char *log_path, time_t started_at, int moves_played,
    int last_player, const Board *board) {
    FILE *log_file;
    time_t now;
    struct tm timestamp;
    char time_buffer[64];
    char player_label;

    log_file = fopen(log_path, "a");
    if (log_file == NULL) {
        return;
    }

    now = time(NULL);
    localtime_r(&now, &timestamp);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &timestamp);
    player_label = last_player == 0 ? 'A' : (last_player == 1 ? 'B' : '-');

    fprintf(log_file, "[%s] elapsed=%ld moves=%d last_player=%c scoreA=%d scoreB=%d\n",
        time_buffer, (long) (now - started_at), moves_played,
        player_label, board->scoreA, board->scoreB);
    fclose(log_file);
}

static void *logger_thread_main(void *argument) {
    MatchLogger *logger = (MatchLogger *) argument;

    for (;;) {
        Board snapshot;
        time_t started_at;
        int moves_played;
        int last_player;
        int running;

        sleep(2);

        pthread_mutex_lock(&logger->mutex);
        snapshot = logger->snapshot;
        started_at = logger->started_at;
        moves_played = logger->moves_played;
        last_player = logger->last_player;
        running = logger->running;
        pthread_mutex_unlock(&logger->mutex);

        append_snapshot(logger->log_path, started_at, moves_played, last_player, &snapshot);

        if (!running) {
            break;
        }
    }

    return NULL;
}

int logger_start(MatchLogger *logger, const char *log_path, const Board *initial_board) {
    FILE *log_file;

    memset(logger, 0, sizeof(*logger));
    logger->running = 1;
    logger->moves_played = 0;
    logger->last_player = -1;
    logger->started_at = time(NULL);
    logger->snapshot = *initial_board;
    snprintf(logger->log_path, sizeof(logger->log_path), "%s", log_path);

    if (pthread_mutex_init(&logger->mutex, NULL) != 0) {
        return -1;
    }

    log_file = fopen(log_path, "w");
    if (log_file == NULL) {
        pthread_mutex_destroy(&logger->mutex);
        return -1;
    }

    fprintf(log_file, "Match started\n");
    fclose(log_file);

    if (pthread_create(&logger->thread, NULL, logger_thread_main, logger) != 0) {
        pthread_mutex_destroy(&logger->mutex);
        return -1;
    }

    return 0;
}

void logger_record_move(MatchLogger *logger, const Board *board, int player) {
    pthread_mutex_lock(&logger->mutex);
    logger->snapshot = *board;
    logger->moves_played++;
    logger->last_player = player;
    pthread_mutex_unlock(&logger->mutex);
}

void logger_stop(MatchLogger *logger) {
    pthread_mutex_lock(&logger->mutex);
    logger->running = 0;
    pthread_mutex_unlock(&logger->mutex);

    pthread_join(logger->thread, NULL);
    pthread_mutex_destroy(&logger->mutex);
}
