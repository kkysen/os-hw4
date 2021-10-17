CC = gcc
CFLAGS = -g -Wall -Werror -Wextra -std=c11 -flto
LDFLAGS = -g -pthread -flto

#SRCS = $(shell fd --extension c)
SRCS = $(shell find . -name '*.c')
OBJS = $(SRCS:.c=.o)
TARGETS = test_supermom

all: $(TARGETS) Makefile

depend: .depend

.depend: $(SRCS)
	rm -f "$@"
	$(CC) $(CFLAGS) -MM $^ > "$@"

include .depend

$(TARGETS): $(OBJS)
$(OBJS):

PHONY += clean
clean:
	rm -rf $(TARGETS) $(OBJS)

.PHONY: $(PHONY)

PHONY += run
run: $(TARGETS)
	sudo --preserve-env ./test_supermom
