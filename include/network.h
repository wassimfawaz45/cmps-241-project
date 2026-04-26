#ifndef NETWORK_H
#define NETWORK_H

typedef struct {
    int socket_fd;
    int local_player;
} SessionConnection;

int network_host(SessionConnection *connection, const char *port);
int network_join(SessionConnection *connection, const char *address, const char *port);
int network_send_move(int socket_fd, int r1, int c1, int r2, int c2);
int network_receive_move(int socket_fd, int *r1, int *c1, int *r2, int *c2);
void network_close(int socket_fd);

#endif
