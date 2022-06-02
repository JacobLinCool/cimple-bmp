#include <stdio.h>

#include "../src/bmp.h"
#include "timing.h"
#define AVERAGE false
#define NUMCOLORS 32
#define WIDTH 256
#define HEIGHT 128
#define STARTX 128
#define STARTY 64

typedef struct Point {
    i32 x;
    i32 y;
} Point;

Point create_Point(i32 x, i32 y) {
    Point p = {x, y};
    return p;
}

static inline i32 get_hash(Point* p) { return p->x ^ p->y; }

static inline bool is_same(Point* p1, Point* p2) { return p1->x == p2->x && p1->y == p2->y; }

static inline i32 diff(Pixel* p1, Pixel* p2) {
    i32 r = (i32)p1->red - p2->red;
    i32 g = (i32)p1->green - p2->green;
    i32 b = (i32)p1->blue - p2->blue;
    return r * r + g * g + b * b;
}

static inline Point* get_neighbors(BMP* img, Point* point, u8* size) {
    Point* neighbors = malloc(sizeof(Point) * 8);

    i8 vecs[][2] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};

    *size = 0;
    for (i8 i = 0; i < 8; i++) {
        i32 x = point->x + vecs[i][0];
        i32 y = point->y + vecs[i][1];
        if (bmp_safe(img, x, y)) {
            neighbors[*size] = create_Point(x, y);
            (*size)++;
        }
    }

    return neighbors;
}

static inline i32 calc_diff(BMP* img, Pixel* pixels, Point* point, Pixel* p) {
    i32    total = 0, min = INT32_MAX;
    u8     size = 0;
    Point* neighbors = get_neighbors(img, point, &size);

    for (i32 i = 0; i < size; i++) {
        Pixel* neighbor = img->pixels[neighbors[i].y][neighbors[i].x];
        if (memcmp(neighbor, &PIXEL_WHITE, sizeof(Pixel)) != 0) {
            i32 val = diff(neighbor, p);
            total += val;
            min = val < min ? val : min;
        }
    }

    return AVERAGE ? total / size : min;
}

i32 rand_cmp(const void* a, const void* b) { return rand() % 2 == 0 ? -1 : 1; }

i32 main() {
    srand(0);
    char* tag = "gradient";
    timing_start(tag);

    Pixel* pixels = malloc(sizeof(Pixel) * (NUMCOLORS * NUMCOLORS * NUMCOLORS));
    u32    pixels_size = 0;
    for (int r = 0; r < NUMCOLORS; r++)
        for (int g = 0; g < NUMCOLORS; g++)
            for (int b = 0; b < NUMCOLORS; b++)
                pixels[pixels_size++] =
                    RGB((r * 255 / (NUMCOLORS - 1) << 16) + (g * 255 / (NUMCOLORS - 1) << 8) +
                        b * 255 / (NUMCOLORS - 1));

    qsort(pixels, pixels_size, sizeof(Pixel), &rand_cmp);

    printf("%s: %Lg ms\n", tag, timing_check(tag));
    return 0;
}
