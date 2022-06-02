# compiler
CC = gcc

# prod flags
FLAGS = -lm

# dev flags
DEV_FLAGS = $(FLAGS) -Wall -Wextra -Werror -fsanitize=address -D DEBUG

# example sources
EXAMPLES = $(wildcard example/*.c)

# example executables
EXECUTABLES = $(EXAMPLES:.c=.out)

.PHONY: list build clean

all: build

list:
	@echo "examples: $(EXAMPLES)"

build: $(EXECUTABLES)

clean:
	rm -f $(EXECUTABLES)

$(EXECUTABLES):
	$(CC) $(FLAGS) -o $@ $(@:.out=.c)
