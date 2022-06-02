#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/bmp.h"
#include "timing.h"
#define SIZE 1024
#define BLOCK_SIZE 16

i32 red_from = 0x40, red_to = 0xFF + 1;
i32 green_from = 0x40, green_to = 0xFF + 1;
i32 blue_from = 0x40, blue_to = 0xFF + 1;
i32 alpha_from = 0x40, alpha_to = 0xC0 + 1;

static inline i32 rand_num(i32 from, i32 to) { return (rand() % (to - from)) + from; }

bool do_draw(BMP* bmp, i32 x, i32 y) {
    u16 alphas = 0;
    if (bmp_safe(bmp, x - 1, y)) {
        alphas += bmp->pixels[y][x - 1]->alpha;
    }
    if (bmp_safe(bmp, x, y - 1)) {
        alphas += bmp->pixels[y - 1][x]->alpha;
    }
    if (bmp_safe(bmp, x + 1, y)) {
        alphas += bmp->pixels[y][x + 1]->alpha;
    }
    if (bmp_safe(bmp, x, y + 1)) {
        alphas += bmp->pixels[y + 1][x]->alpha;
    }

    return rand_num(0, alphas) > 0;
}

i32 main() {
    srand(SIZE / BLOCK_SIZE);

    char* tag_1 = "draw random color blocks";
    timing_start(tag_1);
    BMP* bmp = create_bmp(SIZE, SIZE, PIXEL_TRANSPARENT);

    for (i32 y = 0; y < SIZE; y += BLOCK_SIZE) {
        for (i32 x = 0; x < SIZE; x += BLOCK_SIZE) {
            u8 red = rand_num(red_from, red_to);
            u8 green = rand_num(green_from, green_to);
            u8 blue = rand_num(blue_from, blue_to);
            Bmp.rect(bmp, x, y, BLOCK_SIZE, BLOCK_SIZE, (Pixel){red, green, blue, 0xFF});
        }
    }

    Bmp.save(bmp, "img/random_blocks.bmp", 8, 8, 8, 0);
    Bmp.free(bmp);
    printf("%s: %Lg ms\n", tag_1, timing_check(tag_1));

    char* tag_2 = "draw random color circles";
    timing_start(tag_2);
    BMP* circles_img = create_bmp(SIZE, SIZE, PIXEL_TRANSPARENT);

    i32 count = SIZE * SIZE / (BLOCK_SIZE * BLOCK_SIZE);
    for (i32 i = 0; i < count; i++) {
        Pixel pixel = {rand_num(red_from, red_to), rand_num(green_from, green_to),
                       rand_num(blue_from, blue_to), rand_num(alpha_from, alpha_to)};
        Bmp.circle(circles_img, rand_num(0, SIZE), rand_num(0, SIZE),
                   rand_num(BLOCK_SIZE, BLOCK_SIZE * 4 + 1), pixel);
    }

    Bmp.save(circles_img, "img/random_circles.bmp", 8, 8, 8, 0);
    Bmp.free(circles_img);
    printf("%s: %Lg ms\n", tag_2, timing_check(tag_2));

    return 0;
}
