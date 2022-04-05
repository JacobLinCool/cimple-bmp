/**
 * @file bmp.h
 * @author JacobLinCool (hi@jacoblin.cool)
 * @brief BMP image library.
 * @version 0.1.0
 * @date 2022-04-01
 *
 * @copyright Copyright (c) 2022 JacobLinCool
 * https://github.com/JacobLinCool/cimple-bmp/
 */

#ifndef _CIMPLE_BMP_H
#define _CIMPLE_BMP_H
 // #region Headers
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #endregion

 // #region Type alias for standard types.
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef long double f128;
typedef char* string;
// #endregion

// #region BMP constants and structs.
#define BMP_MAGIC 0x4D42 // "BM"

typedef struct Mask {
    u32 blue;
    u32 green;
    u32 red;
    u32 alpha;
} __attribute__((packed)) Mask;

static Mask Mask_555 = { 0b0111110000000000U, 0b0000001111100000U, 0b0000000000011111U, 0 };
static Mask Mask_565 = { 0b1111100000000000U, 0b0000011111100000U, 0b0000000000011111U, 0 };
static Mask Mask_888 = { 0b11111111U << 0U, 0b11111111U << 8U, 0b11111111U << 16U, 0 };
static Mask Mask_8888 = { 0b11111111U << 0U, 0b11111111U << 8U, 0b11111111U << 16U, 0b11111111U << 24U };

typedef struct Pixel {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} Pixel;

#define PIXEL_BLACK ((Pixel){ 0, 0, 0, 0xFF })
#define PIXEL_WHITE ((Pixel){ 0xFF, 0xFF, 0xFF, 0xFF })
#define PIXEL_RED ((Pixel){ 0xFF, 0, 0, 0xFF })
#define PIXEL_GREEN ((Pixel){ 0, 0xFF, 0, 0xFF })
#define PIXEL_BLUE ((Pixel){ 0, 0, 0xFF, 0xFF })
#define PIXEL_YELLOW ((Pixel){ 0xFF, 0xFF, 0, 0xFF })
#define PIXEL_CYAN ((Pixel){ 0, 0xFF, 0xFF, 0xFF })
#define PIXEL_MAGENTA ((Pixel){ 0xFF, 0, 0xFF, 0xFF })
#define PIXEL_BLACK_TRANSPARENT ((Pixel){ 0, 0, 0, 0 })
#define PIXEL_WHITE_TRANSPARENT ((Pixel){ 0xFF, 0xFF, 0xFF, 0 })
#define PIXEL_RED_TRANSPARENT ((Pixel){ 0xFF, 0, 0, 0 })
#define PIXEL_GREEN_TRANSPARENT ((Pixel){ 0, 0xFF, 0, 0 })
#define PIXEL_BLUE_TRANSPARENT ((Pixel){ 0, 0, 0xFF, 0 })
#define PIXEL_YELLOW_TRANSPARENT ((Pixel){ 0xFF, 0xFF, 0, 0 })
#define PIXEL_CYAN_TRANSPARENT ((Pixel){ 0, 0xFF, 0xFF, 0 })
#define PIXEL_MAGENTA_TRANSPARENT ((Pixel){ 0xFF, 0, 0xFF, 0 })
#define PIXEL_TRANSPARENT PIXEL_BLACK_TRANSPARENT

typedef struct BITMAP_HEADER {
    u16	magic;
    u32	size;
    u16	reserve_1;
    u16	reserve_2;
    u32	offset;
} __attribute__((packed)) BITMAP_HEADER;

typedef struct BITMAPINFOHEADER {
    BITMAP_HEADER file_header;
    u32	header_size;
    i32	width;
    i32	height;
    u16	planes;
    u16	bpp;
    u32	compression;
    u32	bitmap_size;
    i32	res_height;
    i32	res_width;
    u32	palette;
    u32	important;
} __attribute__((packed)) BITMAPINFOHEADER;

typedef struct BITMAPV3INFOHEADER {
    BITMAPINFOHEADER info_header;
    Mask mask;
} __attribute__((packed)) BITMAPV3INFOHEADER;

typedef struct BMP {
    BITMAPV3INFOHEADER* header;
    Pixel*** pixels;
    /** Save image. Use 8,8,8,0 for bits if you don't know what they mean. */
    u8(*save)(struct BMP* bmp, string path, u8 red_bits, u8 green_bits, u8 blue_bits, u8 alpha_bits);
    u64(*fill)(struct BMP* bmp, Pixel pixel);
    u64(*rect)(struct BMP* bmp, i64 from_x, i64 from_y, i64 width, i64 height, Pixel pixel);
    u64(*circle)(struct BMP* bmp, i64 center_x, i64 center_y, i64 radius, Pixel pixel);
    u64(*line)(struct BMP* bmp, i64 from_x, i64 from_y, i64 to_x, i64 to_y, u64 width, Pixel pixel);
    u64(*draw)(struct BMP* bmp, Pixel pixel, bool (*condition)(struct BMP*, i64 x, i64 y));
    bool(*free)(struct BMP* bmp);
} BMP;
// #endregion

// #region Errors.
enum BMP_ERROR {
    BMP_ERROR_NONE = 0,
    BMP_ERROR_FILE_ERROR,
    BMP_ERROR_NOT_BMP,
    BMP_ERROR_INVALID_HEADER,
    BMP_ERROR_NOT_SUPPORTED,
};

static char* BMP_ERROR_MESSAGE[] = {
    "No error",
    "File error",
    "Not a BMP",
    "Invalid BMP header",
    "Not supported",
};
// #endregion

// #region Utilities.
/**
 * @brief Check if a given point is in the bounds of the image.
 *
 * @param bmp the BMP image.
 * @param x the x coordinate.
 * @param y the y coordinate.
 * @return true if the point is in the bounds.
 */
static inline bool bmp_safe(BMP* bmp, i64 x, i64 y) {
    BITMAPINFOHEADER* info_header = (BITMAPINFOHEADER*)bmp->header;
    if (x < 0 || x >= info_header->width || y < 0 || y >= info_header->height) {
        return false;
    }

    return true;
}

/**
 * @brief Blend two pixels.
 *
 * @param a the first pixel.
 * @param b the second pixel.
 * @param weight the weight, between 0 (a) and 1 (b).
 * @return the blended pixel.
 */
static inline Pixel blend(Pixel a, Pixel b, f64 weight) {
    Pixel result;
    result.red = (u8)((f64)a.red * (1.0 - weight) + (f64)b.red * weight);
    result.green = (u8)((f64)a.green * (1.0 - weight) + (f64)b.green * weight);
    result.blue = (u8)((f64)a.blue * (1.0 - weight) + (f64)b.blue * weight);
    result.alpha = (u8)((f64)a.alpha * (1.0 - weight) + (f64)b.alpha * weight);
    return result;
}

static inline Pixel pixel_over(Pixel front, Pixel back) {
    Pixel result;
    f64 front_alpha = (f64)front.alpha / 255.0;
    f64 back_alpha = (f64)back.alpha / 255.0;
    f64 alpha = front_alpha + back_alpha * (1.0 - front_alpha);
    result.alpha = (u8)(alpha * 255.0);
    if (result.alpha == 0) {
        result.red = result.green = result.blue = 0;
    }
    else {
        result.red = (u8)(((f64)front.red * front_alpha + (f64)back.red * back_alpha * (1.0 - front_alpha)) / alpha);
        result.green = (u8)(((f64)front.green * front_alpha + (f64)back.green * back_alpha * (1.0 - front_alpha)) / alpha);
        result.blue = (u8)(((f64)front.blue * front_alpha + (f64)back.blue * back_alpha * (1.0 - front_alpha)) / alpha);
    }
    return result;
}

/**
 * @brief Convert RGBA Hex to Pixel.
 *
 * @param hex 0xAABBCCDD, AA: Red, BB: Green, CC: Blue, DD: Alpha.
 * @return the converted pixel.
 */
static inline Pixel RGBA(u32 hex) {
    Pixel result;
    result.red = (hex >> 24) & 0xFF;
    result.green = (hex >> 16) & 0xFF;
    result.blue = (hex >> 8) & 0xFF;
    result.alpha = hex & 0xFF;
    return result;
}

/**
 * @brief Convert RGB Hex to Pixel.
 *
 * @param hex 0xAABBCC, AA: Red, BB: Green, CC: Blue.
 * @return the converted pixel.
 */
static inline Pixel RGB(u32 hex) {
    Pixel result;
    result.red = (hex >> 16) & 0xFF;
    result.green = (hex >> 8) & 0xFF;
    result.blue = hex & 0xFF;
    result.alpha = 0xFF;
    return result;
}
// #endregion

// #region Drawing.
/**
 * @brief Fill the image with a pixel.
 *
 * @param bmp the image to fill
 * @param pixel the pixel to fill the rectangle with
 * @return the count of pixels that were filled
 */
u64 bmp_fill(BMP* bmp, Pixel pixel) {
    u64 count = 0;

    for (i64 y = 0; y < bmp->header->info_header.height; y++) {
        for (i64 x = 0; x < bmp->header->info_header.width; x++) {
            if (bmp_safe(bmp, x, y)) {
                bmp->pixels[y][x]->red = pixel.red;
                bmp->pixels[y][x]->green = pixel.green;
                bmp->pixels[y][x]->blue = pixel.blue;
                bmp->pixels[y][x]->alpha = pixel.alpha;
                count++;
            }
        }
    }

    return count;
}

/**
 * @brief Copy a part of the source image to the destination image.
 *
 * @param bmp the destination image
 * @param from_x the x coordinate of the top left corner of the rectangle
 * @param from_y the y coordinate of the top left corner of the rectangle
 * @param width the width of the rectangle
 * @param height the height of the rectangle
 * @param source the source image
 * @param source_x the x coordinate of the source rectangle
 * @param source_y the y coordinate of the source rectangle
 * @return the count of pixels copied.
 */
u64 bmp_copy(BMP* bmp, i64 from_x, i64 from_y, i64 width, i64 height, BMP* source, i64 source_x, i64 source_y) {
    u64 count = 0;

    for (i64 y = 0; y < height; y++) {
        for (i64 x = 0; x < width; x++) {
            if (bmp_safe(bmp, from_x + x, from_y + y)) {
                if (bmp_safe(source, source_x + x, source_y + y)) {
                    bmp->pixels[from_y + y][from_x + x]->red = source->pixels[source_y + y][source_x + x]->red;
                    bmp->pixels[from_y + y][from_x + x]->green = source->pixels[source_y + y][source_x + x]->green;
                    bmp->pixels[from_y + y][from_x + x]->blue = source->pixels[source_y + y][source_x + x]->blue;
                    bmp->pixels[from_y + y][from_x + x]->alpha = source->pixels[source_y + y][source_x + x]->alpha;
                    count++;
                }
            }
        }
    }

    return count;
}

/**
 * @brief Draw a rectangle on the image.
 *
 * @param bmp the image to draw on
 * @param from_x the x coordinate of the top left corner of the rectangle
 * @param from_y the y coordinate of the top left corner of the rectangle
 * @param width the width of the rectangle
 * @param height the height of the rectangle
 * @param pixel the pixel to fill the rectangle with
 * @return the count of pixels that were filled
 */
u64 bmp_rect(BMP* bmp, i64 from_x, i64 from_y, i64 width, i64 height, Pixel pixel) {
    u64 count = 0;

    for (i64 y = 0; y < height; y++) {
        for (i64 x = 0; x < width; x++) {
            if (bmp_safe(bmp, from_x + x, from_y + y)) {
                bmp->pixels[from_y + y][from_x + x]->red = pixel.red;
                bmp->pixels[from_y + y][from_x + x]->green = pixel.green;
                bmp->pixels[from_y + y][from_x + x]->blue = pixel.blue;
                bmp->pixels[from_y + y][from_x + x]->alpha = pixel.alpha;
                count++;
            }
        }
    }

    return count;
}

/**
 * @brief Draw a circle on the image.
 *
 * @param bmp the image to draw on
 * @param center_x the x coordinate of the center of the circle
 * @param center_y the y coordinate of the center of the circle
 * @param radius the radius of the circle
 * @param pixel the pixel to fill the circle with
 * @return the count of pixels that were filled
 */
u64 bmp_circle(BMP* bmp, i64 center_x, i64 center_y, i64 radius, Pixel pixel) {
    u64 count = 0;

    for (i64 y = center_y - radius; y <= center_y + radius; y++) {
        for (i64 x = center_x - radius; x <= center_x + radius; x++) {
            if (bmp_safe(bmp, x, y)) {
                if ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y) <= radius * radius) {
                    bmp->pixels[y][x]->red = pixel.red;
                    bmp->pixels[y][x]->green = pixel.green;
                    bmp->pixels[y][x]->blue = pixel.blue;
                    bmp->pixels[y][x]->alpha = pixel.alpha;
                    count++;
                }
            }
        }
    }

    return count;
}

/**
 * @brief Draw a line on the image
 *
 * @param bmp the image to draw on
 * @param from_x the x coordinate of the start of the line
 * @param from_y the y coordinate of the start of the line
 * @param to_x the x coordinate of the end of the line
 * @param to_y the y coordinate of the end of the line
 * @param width the width of the line
 * @param pixel the color of the line
 * @return the count of pixels that were drawn
 */
u64 bmp_line(BMP* bmp, i64 from_x, i64 from_y, i64 to_x, i64 to_y, u64 width, Pixel pixel) {
    u64 count = 0;

    bool steep = llabs(to_y - from_y) > llabs(to_x - from_x);

    if (steep) {
        i64 temp = from_x;
        from_x = from_y;
        from_y = temp;

        temp = to_x;
        to_x = to_y;
        to_y = temp;
    }

    if (from_x > to_x) {
        i64 temp = from_x;
        from_x = to_x;
        to_x = temp;

        temp = from_y;
        from_y = to_y;
        to_y = temp;
    }

    i64 dx = to_x - from_x, dy = llabs(to_y - from_y);
    i64 err = dx / 2;
    i64 step_y = from_y < to_y ? 1 : -1;

    i64 y = from_y;
    for (i64 x = from_x; x <= to_x; x++) {
        count += bmp->circle(bmp, steep ? y : x, steep ? x : y, width, pixel);
        err -= dy;
        if (err < 0) {
            y += step_y;
            err += dx;
        }
    }

    return count;
}

/**
 * @brief Draw something on the image by using a custom function.
 *
 * @param bmp the image to draw on
 * @param pixel the pixel to draw
 * @param condition custom function to determine if the pixel should be drawn
 * @return the count of pixels that were drawn
 */
u64 bmp_draw(BMP* bmp, Pixel pixel, bool (*condition)(BMP*, i64, i64)) {
    u64 count = 0;

    for (i64 y = 0; y < bmp->header->info_header.height; y++) {
        for (i64 x = 0; x < bmp->header->info_header.width; x++) {
            if (bmp_safe(bmp, x, y) && (*condition)(bmp, x, y)) {
                bmp->pixels[y][x]->red = pixel.red;
                bmp->pixels[y][x]->green = pixel.green;
                bmp->pixels[y][x]->blue = pixel.blue;
                bmp->pixels[y][x]->alpha = pixel.alpha;
                count++;
            }
        }
    }

    return count;
}
// #endregion

// #region File IO, instance management.
bool bmp_free(BMP* bmp) {
    if (bmp == NULL) {
        return false;
    }

    if (bmp->header != NULL) {
        free(bmp->header);
    }

    BITMAPINFOHEADER* info_header = (BITMAPINFOHEADER*)bmp->header;

    if (bmp->pixels != NULL) {
        for (i64 y = 0; y < info_header->height; y++) {
            for (i64 x = 0; x < info_header->width; x++) {
                if (bmp->pixels[y][x] != NULL) {
                    free(bmp->pixels[y][x]);
                }
            }

            free(bmp->pixels[y]);
        }

        free(bmp->pixels);
    }

    free(bmp);

    return true;
}

u8 write_bmp(BMP* bmp, string path, u8 red_bits, u8 green_bits, u8 blue_bits, u8 alpha_bits) {
    u16 bpp = red_bits + green_bits + blue_bits + alpha_bits;
    Mask mask;

    if (red_bits == 5 && green_bits == 5 && blue_bits == 5 && alpha_bits == 0) {
        mask = Mask_555;
        bpp = 16;
    }
    else if (red_bits == 5 && green_bits == 6 && blue_bits == 5 && alpha_bits == 0) {
        mask = Mask_565;
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 0) {
        mask = Mask_888;
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 8) {
        mask = Mask_8888;
    }
    else {
        return BMP_ERROR_NOT_SUPPORTED;
    }

    FILE* file = fopen(path, "w");
    if (file == NULL) {
        return BMP_ERROR_FILE_ERROR;
    }

    BITMAPV3INFOHEADER* header = calloc(1, sizeof(BITMAPV3INFOHEADER));
    memcpy(header, bmp->header, sizeof(BITMAPV3INFOHEADER));
    BITMAP_HEADER* file_header = (BITMAP_HEADER*)header;
    BITMAPINFOHEADER* info_header = (BITMAPINFOHEADER*)header;
    info_header->bpp = bpp;
    info_header->bitmap_size = info_header->width * info_header->height * (bpp / 8);
    file_header->size = sizeof(BITMAPV3INFOHEADER) + info_header->bitmap_size;
    header->mask = mask;

    fwrite(header, sizeof(BITMAPV3INFOHEADER), 1, file);

    u64 row_size = ((info_header->width * info_header->bpp + 31) / 32) * 4;
    u64 padding = row_size - (info_header->width * info_header->bpp / 8);
    u8* pad = calloc(padding, sizeof(u8));
    for (i32 y = info_header->height - 1; y >= 0; y--) {
        for (i32 x = 0; x < info_header->width; x++) {
            u32 pixel_data = 0;

            pixel_data |= (u32)((f64)bmp->pixels[y][x]->blue / 0xFF * ((1UL << blue_bits) - 1));
            pixel_data |= (u32)((f64)bmp->pixels[y][x]->green / 0xFF * ((1UL << green_bits) - 1)) << blue_bits;
            pixel_data |= (u32)((f64)bmp->pixels[y][x]->red / 0xFF * ((1UL << red_bits) - 1)) << (blue_bits + green_bits);
            pixel_data |= (u32)((f64)bmp->pixels[y][x]->alpha / 0xFF * ((1UL << alpha_bits) - 1)) << (blue_bits + green_bits + red_bits);

            fwrite(&pixel_data, bpp / 8, 1, file);
        }

        if (padding != 0) {
            fwrite(pad, padding, 1, file);
        }
    }

    fclose(file);
    free(pad), free(header);
    return BMP_ERROR_NONE;
}

static inline void setup_bmp_methods(BMP* bmp) {
    bmp->save = write_bmp;
    bmp->fill = bmp_fill;
    bmp->rect = bmp_rect;
    bmp->circle = bmp_circle;
    bmp->line = bmp_line;
    bmp->draw = bmp_draw;
    bmp->free = bmp_free;
}

BMP* create_bmp(u64 width, u64 height, Pixel pixel) {
    u16 bpp = 24;
    u16 pixel_size = bpp / 8;

    BMP* bmp = calloc(1, sizeof(BMP));
    bmp->header = calloc(1, sizeof(BITMAPV3INFOHEADER));

    BITMAP_HEADER* file_header = (BITMAP_HEADER*)bmp->header;
    file_header->magic = BMP_MAGIC;
    file_header->size = sizeof(BITMAPV3INFOHEADER) + pixel_size * width * height;
    file_header->offset = sizeof(BITMAPV3INFOHEADER);

    BITMAPINFOHEADER* info_header = (BITMAPINFOHEADER*)bmp->header;
    info_header->header_size = sizeof(BITMAPV3INFOHEADER) - sizeof(BITMAP_HEADER);
    info_header->width = width;
    info_header->height = height;
    info_header->planes = 1;
    info_header->bpp = bpp;
    info_header->bitmap_size = pixel_size * width * height;
    info_header->res_height = 9449;
    info_header->res_width = 9449;

    bmp->header->mask = Mask_888;

    bmp->pixels = calloc(height, sizeof(Pixel**));
    for (u64 y = 0; y < height; y++) {
        bmp->pixels[y] = malloc(width * sizeof(Pixel*));
        for (u64 x = 0; x < width; x++) {
            bmp->pixels[y][x] = malloc(sizeof(Pixel));
            bmp->pixels[y][x]->blue = pixel.blue;
            bmp->pixels[y][x]->green = pixel.green;
            bmp->pixels[y][x]->red = pixel.red;
            bmp->pixels[y][x]->alpha = pixel.alpha;
        }
    }

    setup_bmp_methods(bmp);

    return bmp;
}

u8 read_bmp(string path, BMP* bmp) {
    FILE* bmp_file = fopen(path, "rb");
    if (bmp_file == NULL) {
        return BMP_ERROR_FILE_ERROR;
    }

    u64 file_size = 0;
    fseek(bmp_file, 0, SEEK_END);
    file_size = ftell(bmp_file);
    fseek(bmp_file, 0, SEEK_SET);

    u16 magic = 0;
    fread(&magic, sizeof(u16), 1, bmp_file);
    fseek(bmp_file, 0, SEEK_SET);
    if (file_size < 2 || magic != BMP_MAGIC) {
        fclose(bmp_file);
        return BMP_ERROR_NOT_BMP;
    }

    u8* file = malloc(file_size);
    fread(file, file_size, 1, bmp_file);
    fclose(bmp_file);

    bmp = calloc(1, sizeof(BMP));
    bmp->header = calloc(1, sizeof(BITMAPV3INFOHEADER));

    u32 header_size = ((BITMAPINFOHEADER*)file)->header_size + sizeof(BITMAP_HEADER);
    u32 max_header_size = sizeof(BITMAPV3INFOHEADER) - sizeof(BITMAP_HEADER);
    memcpy(bmp->header, file, header_size > max_header_size ? max_header_size : header_size);
    BITMAPINFOHEADER* info_header = (BITMAPINFOHEADER*)bmp->header;

    Mask mask = bmp->header->mask;
    if (mask.red == 0 && mask.green == 0 && mask.blue == 0) {
        switch (info_header->bpp) {
        case 16:
            mask = Mask_555;
            break;
        case 24:
            mask = Mask_888;
            break;
        default:
            free(file), free(bmp->header), free(bmp);
            return BMP_ERROR_INVALID_HEADER;
            break;
        }
    }

    bmp->pixels = calloc(info_header->height, sizeof(Pixel*));
    for (i32 i = 0; i < info_header->height; i++) {
        bmp->pixels[i] = calloc(info_header->width, sizeof(Pixel*));
    }

    u8* data = file + ((BITMAP_HEADER*)bmp->header)->offset;
    u64 offset = 0;
    u64 row_size = ((info_header->width * info_header->bpp + 31) / 32) * 4;
    for (i32 y = 0; y < info_header->height; y++) {
        for (i32 x = 0; x < info_header->width; x++) {
            Pixel* pixel = calloc(1, sizeof(Pixel));

            u32 pixel_data = data[offset];

            pixel->blue = pixel_data & mask.blue;
            pixel->green = pixel_data & mask.green;
            pixel->red = pixel_data & mask.red;
            pixel->alpha = pixel_data & mask.alpha;

            bmp->pixels[info_header->height - 1 - y][x] = pixel;
            offset += info_header->bpp / 8;
        }

        if (offset % row_size != 0) {
            offset += row_size - (offset % row_size);
        }
    }

    setup_bmp_methods(bmp);

    free(file);
    return BMP_ERROR_NONE;
}
// #endregion

#endif // _CIMPLE_BMP_H
