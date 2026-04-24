CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -Iinclude
SRC = src/main.c src/game.c src/board.c src/bot.c
OBJ = $(SRC:.c=.o)
TARGET = dotsandboxes

all: $(TARGET)

$(TARGET): $(OBJ)
	gcc $(CFLAGS) -o $(TARGET) $(OBJ)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
