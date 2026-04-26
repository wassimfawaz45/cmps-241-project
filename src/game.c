#include <stdio.h>
#include <string.h>

#include "board.h"
#include "game.h"
#include "logger.h"
#include "network.h"

typedef enum {
    MODE_HOST,
    MODE_CLIENT
} SessionMode;

typedef struct {
    SessionMode mode;
    const char *address;
    const char *port;
} GameConfig;

static void clear_input_line(void) {
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

static void print_usage(const char *program_name) {
    printf("Usage:\n");
    printf("  %s --host <port>\n", program_name);
    printf("  %s --connect <ip-address> <port>\n", program_name);
}

static int parse_arguments(int argc, char **argv, GameConfig *config) {
    if (argc == 3 && strcmp(argv[1], "--host") == 0) {
        config->mode = MODE_HOST;
        config->address = NULL;
        config->port = argv[2];
        return 0;
    }

    if (argc == 4 && strcmp(argv[1], "--connect") == 0) {
        config->mode = MODE_CLIENT;
        config->address = argv[2];
        config->port = argv[3];
        return 0;
    }

    return -1;
}

static int read_local_move(int player, int *r1, int *c1, int *r2, int *c2) {
    printf("Player %c turn. Enter r1 c1 r2 c2: ", player == 0 ? 'A' : 'B');
    fflush(stdout);
    if (scanf("%d %d %d %d", r1, c1, r2, c2) != 4) {
        printf("Invalid input. Please enter 4 integers.\n");
        clear_input_line();
        return -1;
    }

    return 0;
}

static void announce_turn_result(int player, int claimed) {
    if (claimed > 0) {
        printf("Player %c completed %d box(es) and plays again.\n",
            player == 0 ? 'A' : 'B', claimed);
    }
}

static void announce_winner(const Board *board) {
    char winner = board_winner(board);

    if (winner == 'T') {
        printf("Game over. It is a tie.\n");
    } else {
        printf("Game over. Player %c wins.\n", winner);
    }
}

int game_run(int argc, char **argv) {
    Board board;
    GameConfig config;
    MatchLogger logger;
    SessionConnection connection;
    int current_player = 0;
    int local_player;
    int logger_started = 0;

    if (parse_arguments(argc, argv, &config) != 0) {
        print_usage(argv[0]);
        return 1;
    }

    connection.socket_fd = -1;
    if (config.mode == MODE_HOST) {
        printf("Hosting game on port %s. Waiting for Player B...\n", config.port);
        if (network_host(&connection, config.port) != 0) {
            printf("Failed to host the game.\n");
            return 1;
        }
        printf("Player B connected.\n");
    } else {
        printf("Connecting to %s:%s...\n", config.address, config.port);
        if (network_join(&connection, config.address, config.port) != 0) {
            printf("Failed to connect to the host.\n");
            return 1;
        }
        printf("Connected to the host.\n");
    }

    board_init(&board);
    if (logger_start(&logger, "match_log.txt", &board) == 0) {
        logger_started = 1;
    } else {
        printf("Warning: logger thread could not be started.\n");
    }

    local_player = connection.local_player;
    printf("You are Player %c.\n", local_player == 0 ? 'A' : 'B');

    while (!board_full(&board)) {
        int r1;
        int c1;
        int r2;
        int c2;
        int claimed;

        board_print(&board);

        if (current_player == local_player) {
            if (read_local_move(local_player, &r1, &c1, &r2, &c2) != 0) {
                continue;
            }

            claimed = board_apply_move(&board, local_player, r1, c1, r2, c2);
            if (claimed < 0) {
                printf("Invalid move. Try again.\n");
                continue;
            }

            if (network_send_move(connection.socket_fd, r1, c1, r2, c2) != 0) {
                printf("Connection lost while sending the move.\n");
                break;
            }

            if (logger_started) {
                logger_record_move(&logger, &board, local_player);
            }

            announce_turn_result(local_player, claimed);
            if (claimed == 0) {
                current_player = 1 - current_player;
            }
        } else {
            printf("Waiting for Player %c to move...\n", current_player == 0 ? 'A' : 'B');
            if (network_receive_move(connection.socket_fd, &r1, &c1, &r2, &c2) != 0) {
                printf("Connection lost while waiting for the other player.\n");
                break;
            }

            claimed = board_apply_move(&board, current_player, r1, c1, r2, c2);
            if (claimed < 0) {
                printf("Received an invalid move from the remote player.\n");
                break;
            }

            if (logger_started) {
                logger_record_move(&logger, &board, current_player);
            }

            printf("Player %c played: %d %d %d %d\n",
                current_player == 0 ? 'A' : 'B', r1, c1, r2, c2);
            announce_turn_result(current_player, claimed);
            if (claimed == 0) {
                current_player = 1 - current_player;
            }
        }
    }

    board_print(&board);
    if (board_full(&board)) {
        announce_winner(&board);
    }

    if (logger_started) {
        logger_stop(&logger);
    }
    network_close(connection.socket_fd);
    return 0;
}
