CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc

# Check if taglib is available and add it
TAGLIB  := $(shell pkg-config --libs taglib_c 2>/dev/null)
ifneq ($(TAGLIB),)
    CFLAGS  += -DHAVE_TAGLIB $(shell pkg-config --cflags taglib_c)
    LIBS     = -lncurses -lmpv -ltag_c
else
    LIBS     = -lncurses -lmpv
endif

SRCS    = src/main.c                  \
          src/audio/player.c          \
          src/audio/playlist.c        \
          src/ui/layout.c             \
          src/ui/draw.c               \
          src/ui/colors.c             \
          src/input/keybinds.c        \
          src/state/state.c           \
          src/library/scanner.c       \
          src/library/metadata.c

OBJS    = $(SRCS:.c=.o)
TARGET  = pamp

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(TARGET)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

.PHONY: all clean install uninstall
