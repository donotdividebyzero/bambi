PROG := bambi
SRC_DIRS ?= src
SRCS := $(shell find $(SRC_DIRS) -name "*.c")

CC      := time cc
DEBUG   := -g
CFLAGS  := -Wall -Wextra -Werror -Werror=enum-compare $(DEBUG)
LDFLAGS :=

OBJS   = $(SRCS:.c=.o)
DEPS   = $(SRCS:.c=.d)

.PHONY: all
all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(DEBUG) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(DEPS)

.PHONY: clean cleaner
clean:
	rm -f src/$(OBJS) src/$(DEPS)

cleaner: clean
	rm -rf src/$(PROG)
