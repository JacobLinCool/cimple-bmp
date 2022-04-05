#include "../src/bmp.h"
#include "timing.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define LINE_WIDTH 3.0
#define HEIGHT 1000.0
#define WIDTH (HEIGHT * M_PI)
#define X_OFFSET (WIDTH / 2.0)
#define Y_OFFSET (HEIGHT / 2.0)
#define X_SCALE 100.0
#define Y_SCALE 100.0

bool draw_background(BMP* bmp, i64 x, i64 y) {
    y = (HEIGHT - y) - Y_OFFSET;
    x = x - X_OFFSET;
    if (x % 2 == 0 && y % 2 == 0) {
        return true;
    }
    return false;
}

bool draw_sine(BMP* bmp, i64 x, i64 y) {
    y = (HEIGHT - y) - Y_OFFSET;
    x = x - X_OFFSET;
    if (fabs(y - sin(x / X_SCALE) * Y_SCALE) <= LINE_WIDTH) {
        return true;
    }
    return false;
}

bool draw_cosine(BMP* bmp, i64 x, i64 y) {
    y = (HEIGHT - y) - Y_OFFSET;
    x = x - X_OFFSET;
    if (fabs(y - cos(x / X_SCALE) * Y_SCALE) <= LINE_WIDTH) {
        return true;
    }
    return false;
}

bool draw_tan(BMP* bmp, i64 x, i64 y) {
    y = (HEIGHT - y) - Y_OFFSET;
    x = x - X_OFFSET;
    if (fabs(y - tan(x / X_SCALE) * Y_SCALE) <= LINE_WIDTH) {
        return true;
    }
    return false;
}

bool draw_cot(BMP* bmp, i64 x, i64 y) {
    y = (HEIGHT - y) - Y_OFFSET;
    x = x - X_OFFSET;
    if (fabs(y - 1.0 / tan(x / X_SCALE) * Y_SCALE) <= LINE_WIDTH) {
        return true;
    }
    return false;
}

bool draw_sec(BMP* bmp, i64 x, i64 y) {
    y = (HEIGHT - y) - Y_OFFSET;
    x = x - X_OFFSET;
    if (fabs(y - 1.0 / cos(x / X_SCALE) * Y_SCALE) <= LINE_WIDTH) {
        return true;
    }
    return false;
}

bool draw_csc(BMP* bmp, i64 x, i64 y) {
    y = (HEIGHT - y) - Y_OFFSET;
    x = x - X_OFFSET;
    if (fabs(y - 1.0 / sin(x / X_SCALE) * Y_SCALE) <= LINE_WIDTH) {
        return true;
    }
    return false;
}

i32 main() {
    char* tag = "Drawing";
    timing_start(tag);

    BMP* bmp = create_bmp(WIDTH, HEIGHT, PIXEL_WHITE);

    Pixel PIXEL_GRAY = blend(PIXEL_BLACK, PIXEL_WHITE, 0.5);

    // draw background
    bmp->draw(bmp, PIXEL_YELLOW, &draw_background);

    // draw x-axis and y-axis
    bmp->line(bmp, 0, Y_OFFSET, WIDTH, Y_OFFSET, LINE_WIDTH / 2, PIXEL_GRAY);
    bmp->line(bmp, X_OFFSET, 0, X_OFFSET, HEIGHT, LINE_WIDTH / 2, PIXEL_GRAY);

    // draw sine and cosine
    bmp->draw(bmp, PIXEL_CYAN, &draw_sine);
    bmp->draw(bmp, PIXEL_MAGENTA, &draw_cosine);

    // draw tan and cot
    bmp->draw(bmp, blend(PIXEL_CYAN, PIXEL_YELLOW, 0.5), &draw_tan);
    bmp->draw(bmp, blend(PIXEL_MAGENTA, PIXEL_YELLOW, 0.5), &draw_cot);

    // draw sec and csc
    bmp->draw(bmp, blend(PIXEL_CYAN, PIXEL_BLUE, 0.5), &draw_sec);
    bmp->draw(bmp, blend(PIXEL_MAGENTA, PIXEL_BLUE, 0.5), &draw_csc);

    bmp->save(bmp, "img/plot.bmp", 8, 8, 8, 0);

    bmp->free(bmp);

    printf("%s: %Lg ms\n", tag, timing_check(tag));
    return EXIT_SUCCESS;
}
