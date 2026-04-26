CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -g -D_POSIX_C_SOURCE=200112L -Iinclude
LDFLAGS = -pthread
SRC = src/main.c src/game.c src/board.c src/network.c src/logger.c
OBJ = $(SRC:.c=.o)
TARGET = dotsandboxes

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

run-host: $(TARGET)
	./$(TARGET) --host 5000

run-client: $(TARGET)
	./$(TARGET) --connect 127.0.0.1 5000

clean:
	rm -f $(OBJ) $(TARGET) match_log.txt
