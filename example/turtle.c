#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/bmp.h"
#include "timing.h"
#define SIZE 1024
#define BLOCK 16
#define ISLAND_SIZE 64

bool draw_island_core(BMP* img, i64* x, i64* y, u64 count) {
    if (count > ISLAND_SIZE) {
        return false;
    }

    Pixel Forest = blend(PIXEL_GREEN, PIXEL_BLACK, 0.3);

    Bmp.rect(img, *x, *y, BLOCK, BLOCK, Forest);

    i64 new_x, new_y;
    do {
        switch (rand() % 4) {
            case 0:
                new_x = *x + BLOCK;
                break;
            case 1:
                new_x = *x - BLOCK;
                break;
            case 2:
                new_y = *y + BLOCK;
                break;
            case 3:
                new_y = *y - BLOCK;
                break;
        }
    } while (!bmp_safe(img, new_x, new_y));

    *x = new_x;
    *y = new_y;

    return true;
}

bool draw_island(BMP* img, i64 x, i64 y) {
    Pixel Forest = blend(PIXEL_GREEN, PIXEL_BLACK, 0.3);

    if (memcmp(img->pixels[y][x], &Forest, sizeof(Pixel)) == 0) {
        return false;
    }

    u8 weight = 0;
    if (bmp_safe(img, x - 1, y) && memcmp(img->pixels[y][x - 1], &Forest, sizeof(Pixel)) == 0) {
        weight++;
    }
    if (bmp_safe(img, x + 1, y) && memcmp(img->pixels[y][x + 1], &Forest, sizeof(Pixel)) == 0) {
        weight++;
    }
    if (bmp_safe(img, x, y - 1) && memcmp(img->pixels[y - 1][x], &Forest, sizeof(Pixel)) == 0) {
        weight++;
    }
    if (bmp_safe(img, x, y + 1) && memcmp(img->pixels[y + 1][x], &Forest, sizeof(Pixel)) == 0) {
        weight++;
    }

    return weight > 0;
}

i32 main() {
    srand(time(NULL));
    char* tag = "Map Maker";
    timing_start(tag);

    BMP* image = create_bmp(SIZE, SIZE, PIXEL_BLUE);

    for (i32 i = 0; rand() % 5; i++) {
        i64 x = rand() % (SIZE / BLOCK) * BLOCK;
        i64 y = rand() % (SIZE / BLOCK) * BLOCK;
        Bmp.turtle(image, x, y, &draw_island_core);
    }

    Bmp.draw(image, PIXEL_GREEN, &draw_island);

    Bmp.save(image, "img/map.bmp", 8, 8, 8, 0);

    printf("%s: %Lg ms\n", tag, timing_check(tag));
    return EXIT_SUCCESS;
}
