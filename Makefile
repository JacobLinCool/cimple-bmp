all: test

test:
	gcc -o colors -lm tests/colors.c
	gcc -o plot -lm tests/plot.c
	gcc -o maldives -lm tests/maldives.c
	gcc -o random -lm tests/random.c
	@./colors
	@./plot
	@./maldives
	@./random

.PHONY: all test
