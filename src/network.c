#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "network.h"

static int create_connected_socket(const char *address, const char *port, int passive) {
    struct addrinfo hints;
    struct addrinfo *result;
    struct addrinfo *current;
    int socket_fd = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = passive ? AI_PASSIVE : 0;

    if (getaddrinfo(address, port, &hints, &result) != 0) {
        return -1;
    }

    for (current = result; current != NULL; current = current->ai_next) {
        socket_fd = socket(current->ai_family, current->ai_socktype, current->ai_protocol);
        if (socket_fd < 0) {
            continue;
        }

        if (!passive) {
            if (connect(socket_fd, current->ai_addr, current->ai_addrlen) == 0) {
                break;
            }
        } else {
            int opt = 1;
            setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            if (bind(socket_fd, current->ai_addr, current->ai_addrlen) == 0) {
                break;
            }
        }

        close(socket_fd);
        socket_fd = -1;
    }

    freeaddrinfo(result);
    return socket_fd;
}

static int send_all(int socket_fd, const char *buffer, size_t length) {
    size_t sent = 0;

    while (sent < length) {
        ssize_t result = send(socket_fd, buffer + sent, length - sent, 0);
        if (result <= 0) {
            return -1;
        }
        sent += (size_t) result;
    }

    return 0;
}

static int receive_line(int socket_fd, char *buffer, size_t size) {
    size_t used = 0;

    while (used + 1 < size) {
        char ch;
        ssize_t received = recv(socket_fd, &ch, 1, 0);
        if (received <= 0) {
            return -1;
        }

        buffer[used++] = ch;
        if (ch == '\n') {
            buffer[used] = '\0';
            return 0;
        }
    }

    return -1;
}

int network_host(SessionConnection *connection, const char *port) {
    struct sockaddr_storage client_address;
    socklen_t client_length = sizeof(client_address);
    int listen_fd = create_connected_socket(NULL, port, 1);
    int client_fd;

    if (listen_fd < 0) {
        return -1;
    }

    if (listen(listen_fd, 1) < 0) {
        close(listen_fd);
        return -1;
    }

    client_fd = accept(listen_fd, (struct sockaddr *) &client_address, &client_length);
    close(listen_fd);
    if (client_fd < 0) {
        return -1;
    }

    connection->socket_fd = client_fd;
    connection->local_player = 0;
    return 0;
}

int network_join(SessionConnection *connection, const char *address, const char *port) {
    int socket_fd = create_connected_socket(address, port, 0);

    if (socket_fd < 0) {
        return -1;
    }

    connection->socket_fd = socket_fd;
    connection->local_player = 1;
    return 0;
}

int network_send_move(int socket_fd, int r1, int c1, int r2, int c2) {
    char message[64];
    int written = snprintf(message, sizeof(message), "MOVE %d %d %d %d\n", r1, c1, r2, c2);

    if (written < 0 || (size_t) written >= sizeof(message)) {
        return -1;
    }

    return send_all(socket_fd, message, (size_t) written);
}

int network_receive_move(int socket_fd, int *r1, int *c1, int *r2, int *c2) {
    char message[64];

    if (receive_line(socket_fd, message, sizeof(message)) != 0) {
        return -1;
    }

    if (sscanf(message, "MOVE %d %d %d %d", r1, c1, r2, c2) != 4) {
        return -1;
    }

    return 0;
}

void network_close(int socket_fd) {
    if (socket_fd >= 0) {
        close(socket_fd);
    }
}
