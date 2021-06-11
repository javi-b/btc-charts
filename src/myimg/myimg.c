/**
 * My image library.
 *
 * Javi Bonafonte
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <png.h>

#include "myimg.h"

#define CHAN 4 // Channels of the color mode

/**
 * use this...
 * http://www.labbookpages.co.uk/software/imgProc/libPNG.html
 */

/**
 * Converts HSL color to RGB
 * where 0 <= 'h' < 360, 0 <= 's' < 1, 0 <= 'l' < 1
 */
void hsl_to_rgb(int *r, int *g, int *b, int h, float s, float l) {

    float c, x, m, r0 = 0, g0 = 0, b0 = 0;

    c = (1 - fabs (2 * l - 1)) * s;
    x = c * (1 - fabs (fmodf (h / 60.0, 2) - 1));
    m = l - c / 2;

    if ((h >= 0 && h < 60) || (h >= 300 && h < 360))
        r0 = c;
    else if ((h >= 60 && h < 120) || (h >= 240 && h < 300))
        r0 = x;

    if ((h >= 0 && h < 60) || (h >= 180 && h < 240))
        g0 = x;
    else if (h>= 60 && h < 180)
        g0 = c;

    if ((h >= 120 && h < 180) || (h >= 300 && h < 360))
        b0 = x;
    else if (h>= 180 && h < 300)
        b0 = c;

    *r = round ((r0 + m) * 255);
    *g = round ((g0 + m) * 255);
    *b = round ((b0 + m) * 255);
}

/**
 * Returns integers RGBA image buffer of 'width' by 'height' or NULL if it
 * can't create it.
 */
int *create_image(int width, int height) {

    int *image = (int *) malloc (width * height * CHAN * sizeof (int));
    return image;
}

/**
 * Returns index of image buffer with 'wdith' and 'CHAN' number of channels
 * in 'x','y'.
 */
int get_image_index (int width, int x, int y) {

    return y * width * CHAN + x * CHAN;
}

/**
 * Sets RGBA values in 'index' of 'image'.
 */
void set_rgba (int *image, int width, int x, int y,
        int r, int g, int b, int a) {

    int index = get_image_index (width, x, y);

    image[index] = r;
    image[index + 1] = g;
    image[index + 2] = b;
    image[index + 3] = a;
}

/**
 * Paints background of 'image' buffer with 'widht' and 'height' as RGBA
 * values.
 */
void paint_image_background (int *image, int width, int height,
        int r, int g, int b, int a) {

    int x, y;

    for (x = 0; x < width; x++) {
        for (y = 0; y < height; y++)
            set_rgba (image, width, x, y, r, g, b, a);
    }
}

/**
 * Writes 'image' buffer of 'width' and 'height' in 'path'.
 */
int write_image (int *image, char *path, int width, int height) {

    int code = 0;

    // Opens png file for writing (binary mode)
    FILE *fp = fopen (path, "wb");
    if (fp == NULL) {
        fprintf (stderr, "Couldn't open file '%s'\n", path);
        code = 1;
        goto finalise;
    }

    // Initializes write structure
    png_structp png_ptr = png_create_write_struct (
            PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf (stderr, "Couldn't allocate png write struct\n");
        code = 1;
        goto finalise;
    }

    // Initializes info structure
    png_infop info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        fprintf (stderr, "Couldn't allocate png info struct\n");
        code = 1;
        goto finalise;
    }

    png_init_io (png_ptr, fp);

    // Writes header (8 bit colour depth)
    png_set_IHDR (png_ptr, info_ptr, width, height, 8,
            PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info (png_ptr, info_ptr);


    // Allocates memory for one row (3 bytes per pixel - RGBA)
    png_bytep row = (png_bytep) malloc (width * CHAN * sizeof (png_byte));

    // Writes image data
    int x, y, row_index, image_index, channel;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {

            row_index = x * CHAN;
            image_index = get_image_index (width, x, y);

            for (channel = 0; channel < CHAN; channel++) // For channels...
                row[row_index + channel] = image[image_index + channel];
        }
        png_write_row (png_ptr, row);
    }

    // Ends write
    png_write_end (png_ptr, NULL);

finalise:

    if (fp != NULL)
        fclose (fp);

    if (info_ptr != NULL)
        png_free_data (png_ptr, info_ptr, PNG_FREE_ALL, -1);

    if (png_ptr != NULL)
        png_destroy_write_struct (&png_ptr, (png_infopp) NULL);

    if (row != NULL)
        free (row);

    return code;
}

