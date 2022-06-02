#include <stdio.h>

#include "../src/bmp.h"
#include "timing.h"

i32 main() {
    char* tag_1 = "Read";
    timing_start(tag_1);
    BMP* bmp;
    read_bmp("img/maldives.bmp", &bmp);
    printf("%s: %Lg ms\n", tag_1, timing_check(tag_1));

    Pixel LIGHT_BLUE = blend(blend(PIXEL_BLUE, PIXEL_WHITE, 0.5), PIXEL_WHITE, 0.5);
    Pixel Sun_Center = blend(PIXEL_YELLOW, PIXEL_WHITE, 0.5);
    Pixel Sun_Halo1 = blend(Sun_Center, LIGHT_BLUE, 0.5);
    Pixel Sun_Halo2 = blend(Sun_Halo1, LIGHT_BLUE, 0.5);

    // draw sun
    Bmp.circle(bmp, 150, 150, 100, Sun_Halo2);
    Bmp.circle(bmp, 150, 150, 80, Sun_Halo1);
    Bmp.circle(bmp, 150, 150, 60, Sun_Center);

    // draw clouds
    Bmp.circle(bmp, 500, 150, 60, PIXEL_WHITE);
    Bmp.circle(bmp, 440, 170, 40, PIXEL_WHITE);
    Bmp.circle(bmp, 555, 175, 30, PIXEL_WHITE);

    char* tag_2 = "save 16-bit (555) bitmap";
    timing_start(tag_2);
    Bmp.save(bmp, "img/maldives_555.bmp", 5, 5, 5, 0);
    printf("%s: %Lg ms\n", tag_2, timing_check(tag_2));

    char* tag_3 = "save 16-bit (565) bitmap";
    timing_start(tag_3);
    Bmp.save(bmp, "img/maldives_565.bmp", 5, 6, 5, 0);
    printf("%s: %Lg ms\n", tag_3, timing_check(tag_3));

    char* tag_4 = "save 24-bit (888) bitmap";
    timing_start(tag_4);
    Bmp.save(bmp, "img/maldives_888.bmp", 8, 8, 8, 0);
    printf("%s: %Lg ms\n", tag_4, timing_check(tag_4));

    char* tag_5 = "save 32-bit (8888) bitmap";
    timing_start(tag_5);
    Bmp.save(bmp, "img/maldives_8888.bmp", 8, 8, 8, 8);
    printf("%s: %Lg ms\n", tag_5, timing_check(tag_5));

    return 0;
}
