/**
 * My image library. Uses libpng.
 *
 * When using it always start by calling
 * 'create_img (int width, int height, int pad)' with the size of the image
 * that will be generated and its padding. When the image is ready, write
 * it by calling 'write_img (char *path)' and always finish by calling
 * 'free_img ()'.
 *
 * Javi Bonafonte
 */

#include <stdlib.h>
#include <stdio.h>
#include <png.h>

#include "../util/util.h"
#include "myimg.h"

#define CHAN 4 // Channels of the color mode

static int *Img; // Global integers RGBA image buffer
static int Width, Height, Pad; // Global width, height and padding
                               // of the 'Img'

/**
 * Creates an integers RGBA image buffer of 'width' by 'height' and saves
 * it as a global variable. It also saves the 'width', 'height' and 'pad'
 * of the image.
 *
 * Must be the first function to run of this library!!!
 */
int create_img (int width, int height, int pad) {

    Width = width;
    Height = height;
    Pad = pad;

    Img = (int *) malloc (Width * Height * CHAN * sizeof (int));

    if (Img == NULL) {
        fprintf (stderr, "Couldn't create image buffer\n");
        return 1;
    }

    return 0;
}

/**
 * Returns index of image buffer with 'Width' and 'CHAN' number of channels
 * in 'x','y'.
 */
int get_img_index (int x, int y) {

    return y * Width * CHAN + x * CHAN;
}

/**
 * Sets 'rgba' values in position 'x','y' of 'Img'.
 */
int set_rgba (int x, int y, int r, int g, int b, int a) {

    // If 'Img' buffer hasn't been created...
    if (Img == NULL) {
        fprintf (stderr, "Image buffer hasn't been created.\n");
        return 1;
    }

    int index = get_img_index (x, y);

    Img[index] = r;
    Img[index + 1] = g;
    Img[index + 2] = b;
    Img[index + 3] = a;

    return 0;
}

/**
 * Paints background of 'Img' buffer as 'rgba' values.
 */
int paint_img_background (int r, int g, int b, int a) {

    // If 'Img' buffer hasn't been created...
    if (Img == NULL) {
        fprintf (stderr, "Image buffer hasn't been created.\n");
        return 1;
    }

    int x, y;

    for (x = 0; x < Width; x++) {
        for (y = 0; y < Height; y++)
            set_rgba (x, y, r, g, b, a);
    }

    return 0;
}

/**
 * Paints y axis lines on 'Img' from 'min' to 'max'. The color is 'rgba'.
 *
 *      - If the 'scale' is linear, each line is at 'y + step'.
 *      - If the 'scale' is logarithmic, each line is at 'y * step'.
 */
int paint_y_axis (float min, float max, int scale, float step,
        int r, int g, int b, int a) {

    // If 'Img' buffer hasn't been created...
    if (Img == NULL) {
        fprintf (stderr, "Image buffer hasn't been created.\n");
        return 1;
    }

    float scaled_min = apply_scale (scale, min);
    float scaled_max = apply_scale (scale, max);
    int x, j;
    float y;

    y = min;
    while (y <= max) {

        j = Height - Pad -
            (apply_scale (scale, y) - scaled_min) * (Height - 2 * Pad)
            / (scaled_max - scaled_min);

        if (j == Height)
            j--;

        for (x = Pad; x < Width - Pad; x++)
            set_rgba (x, j, r, g, b, a);

        switch (scale) {
            case linear:
            default:
                y += step;
                break;
            case logarithmic:
                y *= step;
                break;
        }
    }

    return 0;
}

/**
 * Writes 'Img' buffer in 'path'.
 *
 * I used this...
 * http://www.labbookpages.co.uk/software/imgProc/libPNG.html
 */
int write_img (char *path) {

    // If 'Img' buffer hasn't been created...
    if (Img == NULL) {
        fprintf (stderr, "Image buffer hasn't been created.\n");
        return 1;
    }

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
    png_set_IHDR (png_ptr, info_ptr, Width, Height, 8,
            PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info (png_ptr, info_ptr);


    // Allocates memory for one row (3 bytes per pixel - RGBA)
    png_bytep row = (png_bytep) malloc (Width * CHAN * sizeof (png_byte));

    // Writes image data
    int x, y, row_index, img_index, channel;

    for (y = 0; y < Height; y++) {
        for (x = 0; x < Width; x++) {

            row_index = x * CHAN;
            img_index = get_img_index (x, y);

            for (channel = 0; channel < CHAN; channel++) // For channels...
                row[row_index + channel] = Img[img_index + channel];
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

/**
 * Frees 'Img' buffer.
 *
 * Must be the last function to run of this library!!!
 */
void free_img () {

    if (Img != NULL)
        free (Img);
}

