#include <stdio.h>

#include "../src/bmp.h"
#include "timing.h"

i32 main() {
    char* tag_1 = "create 810,000 (900x900) bitmap";
    timing_start(tag_1);
    BMP* bmp = create_bmp(900, 900, PIXEL_TRANSPARENT);
    printf("%s: %Lg ms\n", tag_1, timing_check(tag_1));

    char* tag_2 = "rect 135,000 (450x300) with PIXEL_WHITE";
    timing_start(tag_2);
    Bmp.rect(bmp, 450, 0, 450, 300, PIXEL_WHITE);
    printf("%s: %Lg ms\n", tag_2, timing_check(tag_2));

    char* tag_3 = "rect 90,000 (300x300) with PIXEL_RED";
    timing_start(tag_3);
    Bmp.rect(bmp, 0, 300, 300, 300, PIXEL_RED);
    printf("%s: %Lg ms\n", tag_3, timing_check(tag_3));

    Bmp.rect(bmp, 300, 300, 300, 300, PIXEL_GREEN);
    Bmp.rect(bmp, 600, 300, 300, 300, PIXEL_BLUE);

    Bmp.rect(bmp, 0, 600, 300, 300, PIXEL_YELLOW);
    Bmp.rect(bmp, 300, 600, 300, 300, PIXEL_CYAN);
    Bmp.rect(bmp, 600, 600, 300, 300, PIXEL_MAGENTA);

    Bmp.circle(bmp, 450, 150, 100, RGBA(0x00FF7F80));

    Pixel Orange = RGB(0xFFA500);
    Bmp.line(bmp, 300, 100, 600, 100, 0, Orange);
    Bmp.line(bmp, 300, 120, 600, 180, 1, Orange);
    Bmp.line(bmp, 300, 180, 600, 120, 2, Orange);
    Bmp.line(bmp, 600, 200, 300, 200, 3, Orange);

    char* tag_4 = "save 16-bit (555) bitmap";
    timing_start(tag_4);
    Bmp.save(bmp, "img/colors_555.bmp", 5, 5, 5, 0);
    printf("%s: %Lg ms\n", tag_4, timing_check(tag_4));

    char* tag_5 = "save 16-bit (565) bitmap";
    timing_start(tag_5);
    Bmp.save(bmp, "img/colors_565.bmp", 5, 6, 5, 0);
    printf("%s: %Lg ms\n", tag_5, timing_check(tag_5));

    char* tag_6 = "save 24-bit (888) bitmap";
    timing_start(tag_6);
    Bmp.save(bmp, "img/colors_888.bmp", 8, 8, 8, 0);
    printf("%s: %Lg ms\n", tag_6, timing_check(tag_6));

    char* tag_7 = "save 32-bit (8888) bitmap";
    timing_start(tag_7);
    Bmp.save(bmp, "img/colors_8888.bmp", 8, 8, 8, 8);
    printf("%s: %Lg ms\n", tag_7, timing_check(tag_7));

    char* tag_8 = "free 810,000 (900x900) bitmap";
    timing_start(tag_8);
    Bmp.free(bmp);
    printf("%s: %Lg ms\n", tag_8, timing_check(tag_8));

    return 0;
}
