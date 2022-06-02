/**
 * @file bmp.h
 * @author JacobLinCool (hi@jacoblin.cool)
 * @brief BMP image library.
 * @version 2.0.0
 * @date 2022-06-02
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

// #region BMP constants and structs.
#define BMP_MAGIC 0x4D42  // "BM"

typedef struct Mask {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    uint32_t alpha;
} __attribute__((packed)) Mask;

static Mask Mask_555 = {0x1FU << 10U, 0x1FU << 5U, 0x1FU << 0U, 0};
static Mask Mask_565 = {0b1111100000000000U, 0b0000011111100000U, 0b0000000000011111U, 0};
static Mask Mask_888 = {0xFFU << 16U, 0xFFU << 8U, 0xFFU << 0U, 0};
static Mask Mask_8888 = {0xFFU << 16U, 0xFFU << 8U, 0xFFU << 0U, 0xFFU << 24U};

typedef struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} Pixel;

#define PIXEL_BLACK ((Pixel){0, 0, 0, 0xFF})
#define PIXEL_WHITE ((Pixel){0xFF, 0xFF, 0xFF, 0xFF})
#define PIXEL_RED ((Pixel){0xFF, 0, 0, 0xFF})
#define PIXEL_GREEN ((Pixel){0, 0xFF, 0, 0xFF})
#define PIXEL_BLUE ((Pixel){0, 0, 0xFF, 0xFF})
#define PIXEL_YELLOW ((Pixel){0xFF, 0xFF, 0, 0xFF})
#define PIXEL_CYAN ((Pixel){0, 0xFF, 0xFF, 0xFF})
#define PIXEL_MAGENTA ((Pixel){0xFF, 0, 0xFF, 0xFF})
#define PIXEL_BLACK_TRANSPARENT ((Pixel){0, 0, 0, 0})
#define PIXEL_WHITE_TRANSPARENT ((Pixel){0xFF, 0xFF, 0xFF, 0})
#define PIXEL_RED_TRANSPARENT ((Pixel){0xFF, 0, 0, 0})
#define PIXEL_GREEN_TRANSPARENT ((Pixel){0, 0xFF, 0, 0})
#define PIXEL_BLUE_TRANSPARENT ((Pixel){0, 0, 0xFF, 0})
#define PIXEL_YELLOW_TRANSPARENT ((Pixel){0xFF, 0xFF, 0, 0})
#define PIXEL_CYAN_TRANSPARENT ((Pixel){0, 0xFF, 0xFF, 0})
#define PIXEL_MAGENTA_TRANSPARENT ((Pixel){0xFF, 0, 0xFF, 0})
#define PIXEL_TRANSPARENT PIXEL_BLACK_TRANSPARENT

typedef struct BITMAP_HEADER {
    uint16_t magic;
    uint32_t size;
    uint16_t reserve_1;
    uint16_t reserve_2;
    uint32_t offset;
} __attribute__((packed)) BITMAP_HEADER;

typedef struct BITMAPINFOHEADER {
    BITMAP_HEADER file_header;
    uint32_t      header_size;
    int32_t       width;
    int32_t       height;
    uint16_t      planes;
    uint16_t      bpp;
    uint32_t      compression;
    uint32_t      bitmap_size;
    int32_t       res_height;
    int32_t       res_width;
    uint32_t      palette;
    uint32_t      important;
} __attribute__((packed)) BITMAPINFOHEADER;

typedef struct BITMAPV3INFOHEADER {
    BITMAPINFOHEADER info_header;
    Mask             mask;
} __attribute__((packed)) BITMAPV3INFOHEADER;

typedef struct BMP {
    BITMAPV3INFOHEADER* header;
    Pixel***            pixels;
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
    "No error", "File error", "Not a BMP", "Invalid BMP header", "Not supported",
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
static inline bool bmp_safe(BMP* bmp, int64_t x, int64_t y) {
    BITMAPINFOHEADER const* info_header = (BITMAPINFOHEADER*)bmp->header;
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
static inline Pixel blend(Pixel a, Pixel b, double weight) {
    Pixel result;
    result.red = (uint8_t)((double)a.red * (1.0 - weight) + (double)b.red * weight);
    result.green = (uint8_t)((double)a.green * (1.0 - weight) + (double)b.green * weight);
    result.blue = (uint8_t)((double)a.blue * (1.0 - weight) + (double)b.blue * weight);
    result.alpha = (uint8_t)((double)a.alpha * (1.0 - weight) + (double)b.alpha * weight);
    return result;
}

static inline Pixel pixel_over(Pixel front, Pixel back) {
    Pixel  result;
    double front_alpha = (double)front.alpha / 255.0;
    double back_alpha = (double)back.alpha / 255.0;
    double alpha = front_alpha + back_alpha * (1.0 - front_alpha);
    result.alpha = (uint8_t)(alpha * 255.0);
    if (result.alpha == 0) {
        result.red = result.green = result.blue = 0;
    } else {
        result.red = (uint8_t)(((double)front.red * front_alpha +
                                (double)back.red * back_alpha * (1.0 - front_alpha)) /
                               alpha);
        result.green = (uint8_t)(((double)front.green * front_alpha +
                                  (double)back.green * back_alpha * (1.0 - front_alpha)) /
                                 alpha);
        result.blue = (uint8_t)(((double)front.blue * front_alpha +
                                 (double)back.blue * back_alpha * (1.0 - front_alpha)) /
                                alpha);
    }
    return result;
}

/**
 * @brief Convert RGBA Hex to Pixel.
 *
 * @param hex 0xAABBCCDD, AA: Red, BB: Green, CC: Blue, DD: Alpha.
 * @return the converted pixel.
 */
static inline Pixel RGBA(uint32_t hex) {
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
static inline Pixel RGB(uint32_t hex) {
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
uint64_t bmp_fill(BMP* bmp, Pixel pixel) {
    uint64_t count = 0;

    for (int64_t y = 0; y < bmp->header->info_header.height; y++) {
        for (int64_t x = 0; x < bmp->header->info_header.width; x++) {
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
uint64_t bmp_copy(BMP* bmp, int64_t from_x, int64_t from_y, int64_t width, int64_t height,
                  BMP* source, int64_t source_x, int64_t source_y) {
    uint64_t count = 0;

    for (int64_t y = 0; y < height; y++) {
        for (int64_t x = 0; x < width; x++) {
            if (bmp_safe(bmp, from_x + x, from_y + y)) {
                if (bmp_safe(source, source_x + x, source_y + y)) {
                    bmp->pixels[from_y + y][from_x + x]->red =
                        source->pixels[source_y + y][source_x + x]->red;
                    bmp->pixels[from_y + y][from_x + x]->green =
                        source->pixels[source_y + y][source_x + x]->green;
                    bmp->pixels[from_y + y][from_x + x]->blue =
                        source->pixels[source_y + y][source_x + x]->blue;
                    bmp->pixels[from_y + y][from_x + x]->alpha =
                        source->pixels[source_y + y][source_x + x]->alpha;
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
uint64_t bmp_rect(BMP* bmp, int64_t from_x, int64_t from_y, int64_t width, int64_t height,
                  Pixel pixel) {
    uint64_t count = 0;

    for (int64_t y = 0; y < height; y++) {
        for (int64_t x = 0; x < width; x++) {
            if (bmp_safe(bmp, from_x + x, from_y + y)) {
                Pixel blended = pixel_over(pixel, *bmp->pixels[from_y + y][from_x + x]);
                bmp->pixels[from_y + y][from_x + x]->red = blended.red;
                bmp->pixels[from_y + y][from_x + x]->green = blended.green;
                bmp->pixels[from_y + y][from_x + x]->blue = blended.blue;
                bmp->pixels[from_y + y][from_x + x]->alpha = blended.alpha;
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
uint64_t bmp_circle(BMP* bmp, int64_t center_x, int64_t center_y, int64_t radius, Pixel pixel) {
    uint64_t count = 0;

    for (int64_t y = center_y - radius; y <= center_y + radius; y++) {
        for (int64_t x = center_x - radius; x <= center_x + radius; x++) {
            if (bmp_safe(bmp, x, y) &&
                ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y) <=
                 radius * radius)) {
                Pixel blended = pixel_over(pixel, *bmp->pixels[y][x]);
                bmp->pixels[y][x]->red = blended.red;
                bmp->pixels[y][x]->green = blended.green;
                bmp->pixels[y][x]->blue = blended.blue;
                bmp->pixels[y][x]->alpha = blended.alpha;
                count++;
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
uint64_t bmp_line(BMP* bmp, int64_t from_x, int64_t from_y, int64_t to_x, int64_t to_y,
                  uint64_t width, Pixel pixel) {
    uint64_t count = 0;

    bool steep = llabs(to_y - from_y) > llabs(to_x - from_x);

    if (steep) {
        int64_t temp = from_x;
        from_x = from_y;
        from_y = temp;

        temp = to_x;
        to_x = to_y;
        to_y = temp;
    }

    if (from_x > to_x) {
        int64_t temp = from_x;
        from_x = to_x;
        to_x = temp;

        temp = from_y;
        from_y = to_y;
        to_y = temp;
    }

    int64_t dx = to_x - from_x, dy = llabs(to_y - from_y);
    int64_t err = dx / 2;
    int64_t step_y = from_y < to_y ? 1 : -1;

    int64_t y = from_y;
    for (int64_t x = from_x; x <= to_x; x++) {
        count += bmp_circle(bmp, steep ? y : x, steep ? x : y, width, pixel);
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
uint64_t bmp_draw(BMP* bmp, Pixel pixel, bool (*condition)(BMP*, int64_t, int64_t)) {
    uint64_t count = 0;

    for (int64_t y = 0; y < bmp->header->info_header.height; y++) {
        for (int64_t x = 0; x < bmp->header->info_header.width; x++) {
            if (bmp_safe(bmp, x, y) && (*condition)(bmp, x, y)) {
                Pixel blended = pixel_over(pixel, *bmp->pixels[y][x]);
                bmp->pixels[y][x]->red = blended.red;
                bmp->pixels[y][x]->green = blended.green;
                bmp->pixels[y][x]->blue = blended.blue;
                bmp->pixels[y][x]->alpha = blended.alpha;
                count++;
            }
        }
    }

    return count;
}

uint64_t bmp_turtle(BMP* bmp, int64_t start_x, int64_t start_y,
                    bool (*turtle)(BMP*, int64_t*, int64_t*, uint64_t)) {
    uint64_t count = 0;

    int64_t x = start_x, y = start_y;
    while (turtle(bmp, &x, &y, count)) {
        count++;
    }

    return count;
}
// #endregion

// #region File IO, instance management.
bool bmp_free(BMP* bmp) {
    if (bmp == NULL) {
        return false;
    }

    BITMAPINFOHEADER const* info_header = (BITMAPINFOHEADER*)bmp->header;

    if (bmp->pixels != NULL) {
        for (int64_t y = 0; y < info_header->height; y++) {
            for (int64_t x = 0; x < info_header->width; x++) {
                if (bmp->pixels[y][x] != NULL) {
                    free(bmp->pixels[y][x]);
                }
            }

            free(bmp->pixels[y]);
        }

        free(bmp->pixels);
    }

    if (bmp->header != NULL) {
        free(bmp->header);
    }

    free(bmp);

    return true;
}

uint8_t write_bmp(BMP* bmp, char const* path, uint8_t red_bits, uint8_t green_bits,
                  uint8_t blue_bits, uint8_t alpha_bits) {
    uint16_t bpp = red_bits + green_bits + blue_bits + alpha_bits;
    Mask     mask;

    if (red_bits == 5 && green_bits == 5 && blue_bits == 5 && alpha_bits == 0) {
        mask = Mask_555;
        bpp = 16;
    } else if (red_bits == 5 && green_bits == 6 && blue_bits == 5 && alpha_bits == 0) {
        mask = Mask_565;
    } else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 0) {
        mask = Mask_888;
    } else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 8) {
        mask = Mask_8888;
    } else {
        return BMP_ERROR_NOT_SUPPORTED;
    }

    FILE* file = fopen(path, "w");
    if (file == NULL) {
        return BMP_ERROR_FILE_ERROR;
    }

    int32_t  width = bmp->header->info_header.width;
    int32_t  height = bmp->header->info_header.height;
    int32_t  row_size = ((width * bpp + 31) / 32) * 4;
    uint8_t  pixel_size = bpp / 8;

    BITMAPV3INFOHEADER* header = calloc(1, sizeof(BITMAPV3INFOHEADER));
    BITMAP_HEADER*      file_header = (BITMAP_HEADER*)header;
    BITMAPINFOHEADER*   info_header = (BITMAPINFOHEADER*)header;

    header->mask = mask;

    info_header->header_size = sizeof(BITMAPV3INFOHEADER) - sizeof(BITMAP_HEADER);
    info_header->width = width;
    info_header->height = height;
    info_header->planes = 1;
    info_header->bpp = bpp;
    info_header->compression = 3;
    info_header->bitmap_size = abs(height) * row_size;
    info_header->res_height = 9449;
    info_header->res_width = 9449;

    file_header->magic = BMP_MAGIC;
    file_header->offset = sizeof(BITMAPV3INFOHEADER);
    file_header->size = sizeof(BITMAPV3INFOHEADER) + info_header->bitmap_size;

    if (alpha_bits == 0 && bpp == 24) {
        info_header->compression = 0;
    }

    fwrite(header, sizeof(BITMAPV3INFOHEADER), 1, file);

    uint64_t padding_size = row_size - (width * pixel_size);
    uint8_t* padding = calloc(padding_size, sizeof(uint8_t));
    for (int32_t y = info_header->height - 1; y >= 0; y--) {
        for (int32_t x = 0; x < info_header->width; x++) {
            uint32_t pixel_data = 0;

            pixel_data |=
                (uint32_t)((double)bmp->pixels[y][x]->blue / 0xFF * ((1UL << blue_bits) - 1));
            pixel_data |=
                (uint32_t)((double)bmp->pixels[y][x]->green / 0xFF * ((1UL << green_bits) - 1))
                << blue_bits;
            pixel_data |=
                (uint32_t)((double)bmp->pixels[y][x]->red / 0xFF * ((1UL << red_bits) - 1))
                << (blue_bits + green_bits);
            pixel_data |=
                (uint32_t)((double)bmp->pixels[y][x]->alpha / 0xFF * ((1UL << alpha_bits) - 1))
                << (blue_bits + green_bits + red_bits);

            fwrite(&pixel_data, pixel_size, 1, file);
        }

        fwrite(padding, padding_size, 1, file);
    }

    fclose(file);
    free(padding), free(header);
    return BMP_ERROR_NONE;
}

BMP* create_bmp(uint32_t width, uint32_t height, Pixel pixel) {
    uint16_t bpp = 24;
    uint16_t pixel_size = bpp / 8;

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
    for (uint64_t y = 0; y < height; y++) {
        bmp->pixels[y] = malloc(width * sizeof(Pixel*));
        for (uint64_t x = 0; x < width; x++) {
            bmp->pixels[y][x] = malloc(sizeof(Pixel));
            bmp->pixels[y][x]->blue = pixel.blue;
            bmp->pixels[y][x]->green = pixel.green;
            bmp->pixels[y][x]->red = pixel.red;
            bmp->pixels[y][x]->alpha = pixel.alpha;
        }
    }

    return bmp;
}

static inline uint8_t get_shift(uint32_t mask) {
    if (mask == 0) {
        return 0;
    }
    uint8_t shift = 0;
    while (mask ^ 0x1) {
        mask >>= 1;
        shift++;
    }
    return shift;
}

uint8_t read_bmp(char const* path, BMP** bmp) {
    FILE* bmp_file = fopen(path, "rb");
    if (bmp_file == NULL) {
        return BMP_ERROR_FILE_ERROR;
    }

    uint64_t file_size = 0;
    fseek(bmp_file, 0, SEEK_END);
    file_size = ftell(bmp_file);
    fseek(bmp_file, 0, SEEK_SET);

    uint16_t magic = 0;
    fread(&magic, sizeof(uint16_t), 1, bmp_file);
    fseek(bmp_file, 0, SEEK_SET);
    if (file_size < 2 || magic != BMP_MAGIC) {
        fclose(bmp_file);
        return BMP_ERROR_NOT_BMP;
    }

    uint8_t* file = malloc(file_size);
    fread(file, file_size, 1, bmp_file);
    fclose(bmp_file);

    *bmp = calloc(1, sizeof(BMP));
    (*bmp)->header = calloc(1, sizeof(BITMAPV3INFOHEADER));

    uint32_t header_size = ((BITMAPINFOHEADER*)file)->header_size + sizeof(BITMAP_HEADER);
    uint32_t max_header_size = sizeof(BITMAPV3INFOHEADER) - sizeof(BITMAP_HEADER);
    memcpy((*bmp)->header, file, header_size > max_header_size ? max_header_size : header_size);
    BITMAPINFOHEADER const* info_header = (BITMAPINFOHEADER*)(*bmp)->header;

    Mask mask = (*bmp)->header->mask;
    if (info_header->compression == 0) {
        switch (info_header->bpp) {
            case 16:
                mask = Mask_555;
                break;
            case 24:
                mask = Mask_888;
                break;
            case 32:
                mask = Mask_8888;
                break;
            default:
                free(file), free((*bmp)->header), free((*bmp));
                return BMP_ERROR_INVALID_HEADER;
                break;
        }
    }

    uint8_t red_shift = get_shift(mask.red);
    uint8_t green_shift = get_shift(mask.green);
    uint8_t blue_shift = get_shift(mask.blue);
    uint8_t alpha_shift = get_shift(mask.alpha);
    if (mask.alpha == 0) {
        red_shift -= blue_shift;
        green_shift -= blue_shift;
        blue_shift = 0;
    }

    (*bmp)->pixels = calloc(info_header->height, sizeof(Pixel**));
    for (int32_t i = 0; i < info_header->height; i++) {
        (*bmp)->pixels[i] = calloc(info_header->width, sizeof(Pixel*));
    }

    uint8_t* data = file + ((BITMAP_HEADER*)(*bmp)->header)->offset;
    int32_t  offset = 0;
    int32_t  row_size = ((info_header->width * info_header->bpp + 31) / 32) * 4;
    for (int32_t y = 0; y < info_header->height; y++) {
        for (int32_t x = 0; x < info_header->width; x++) {
            Pixel* pixel = calloc(1, sizeof(Pixel));

            uint32_t pixel_data = *(uint32_t*)(data + offset);

            pixel->blue = (pixel_data & mask.blue) >> blue_shift;
            pixel->green = (pixel_data & mask.green) >> green_shift;
            pixel->red = (pixel_data & mask.red) >> red_shift;
            pixel->alpha = mask.alpha ? ((pixel_data & mask.alpha) >> alpha_shift) : 0xFF;

            (*bmp)->pixels[info_header->height - 1 - y][x] = pixel;
            offset += info_header->bpp / 8;
        }

        if (offset % row_size != 0) {
            offset += row_size - (offset % row_size);
        }
    }

    free(file);
    return BMP_ERROR_NONE;
}
// #endregion

struct {
    BMP* (*create)(uint32_t width, uint32_t height, Pixel pixel);
    uint8_t (*read)(char const* path, BMP** bmp);
    bool (*safe)(BMP* bmp, int64_t x, int64_t y);
    /** Save image. Use 8,8,8,0 for bits if you don't know what they mean. */
    uint8_t (*save)(struct BMP* bmp, char const* path, uint8_t red_bits, uint8_t green_bits,
                    uint8_t blue_bits, uint8_t alpha_bits);
    uint64_t (*fill)(struct BMP* bmp, Pixel pixel);
    uint64_t (*rect)(struct BMP* bmp, int64_t from_x, int64_t from_y, int64_t width, int64_t height,
                     Pixel pixel);
    uint64_t (*circle)(struct BMP* bmp, int64_t center_x, int64_t center_y, int64_t radius,
                       Pixel pixel);
    uint64_t (*line)(struct BMP* bmp, int64_t from_x, int64_t from_y, int64_t to_x, int64_t to_y,
                     uint64_t width, Pixel pixel);
    uint64_t (*draw)(struct BMP* bmp, Pixel pixel,
                     bool (*condition)(struct BMP* bmp, int64_t x, int64_t y));
    uint64_t (*turtle)(struct BMP* bmp, int64_t start_x, int64_t start_y,
                       bool (*turtle)(struct BMP* bmp, int64_t* x, int64_t* y, uint64_t count));
    bool (*free)(struct BMP* bmp);
} Bmp = {
    .create = create_bmp,
    .read = read_bmp,
    .safe = bmp_safe,
    .save = write_bmp,
    .fill = bmp_fill,
    .rect = bmp_rect,
    .circle = bmp_circle,
    .line = bmp_line,
    .draw = bmp_draw,
    .turtle = bmp_turtle,
    .free = bmp_free,
};

#endif  // _CIMPLE_BMP_H
