TARGET = pong
CC = gcc

SRC_DIRS = codigo cli-lib/src
INCLUDE_DIRS = cli-lib/include
CFLAGS = $(foreach d, $(INCLUDE_DIRS), -I$d) -Wall -Wextra -std=c11

SRC = $(wildcard $(addsuffix /*.c, $(SRC_DIRS)))
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)
