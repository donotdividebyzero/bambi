PROG := brumbrum
SRC_DIRS ?= ./src
SRCS := $(shell find $(SRC_DIRS) -name "*.c")

CC      := cc
CFLAGS  := -Wall -Wextra -Werror -Werror=enum-compare
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
	rm -f $(OBJS) $(DEPS)

cleaner: clean
	rm -rf $(PROG)

check: all
	./$(PROG) tests

debug: DEBUG = -DDEBUG

debug: $(PROG)