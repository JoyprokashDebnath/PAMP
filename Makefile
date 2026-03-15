CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc
LIBS    = -lncurses -lmpv

# All source files
SRCS    = src/main.c \
          src/audio/player.c

OBJS    = $(SRCS:.c=.o)
TARGET  = pamp

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(TARGET)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
