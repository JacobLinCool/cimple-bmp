all: test

test:
	gcc -o colors -lm tests/colors.c
	gcc -o plot -lm tests/plot.c
	@./colors
	@./plot

.PHONY: all test
