/**
 * My image processing library.
 *
 * Javi Bonafonte
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MagickWand/MagickWand.h>

#include "../util/util.h"
#include "myimgproc.h"

#define FONT "Unifont" // Font family
#define FONT_SIZE 16 // Font size (height)
#define FONT_WIDTH 8 // Font width

/**
 * Reads 'img_path' into 'magick_wand'.
 */
int magick_read_img (MagickWand *magick_wand, char *img_path) {

    int code = 0;
    MagickBooleanType status;

    // Reads image
    status = MagickReadImage (magick_wand, img_path);
    if (status == MagickFalse) {
        fprintf (stderr, "Couldn't read image '%s'\n", img_path);
        code = 1;
    }

    return code;
}

/**
 * Uses 'magick_wand' to write image to 'img_path'.
 */
int magick_write_img (MagickWand *magick_wand, char *img_path) {

    int code = 0;
    MagickBooleanType status;

    // Writes image
    status = MagickWriteImage (magick_wand, img_path);
    if (status == MagickFalse) {
        fprintf (stderr, "Couldn't write image '%s'\n", img_path);
        code = 1;
    }

    return code;
}

/**
 * Annotates 'text' of 'color' on image in 'magick_wand' on position
 * 'x','y' and 'angle'.
 *
 * 'x' on the left of the text.
 * 'y' is the baseline of the text.
 */
int annotate_img (MagickWand *magick_wand, double x, double y,
        double angle, char *text, char *color) {

    int code = 0;
    MagickBooleanType status;

    DrawingWand *drawing_wand;
    PixelWand *pixel_wand;

    drawing_wand = NewDrawingWand ();
    pixel_wand = NewPixelWand ();

    // Sets drawing wand
    DrawSetTextAntialias (drawing_wand, 0);
    // (list available fonts in your system running 'convert -list font')
    DrawSetFont (drawing_wand, FONT);
    DrawSetFontSize (drawing_wand, FONT_SIZE);
    PixelSetColor (pixel_wand, color);
    DrawSetFillColor (drawing_wand, pixel_wand);

    // Annotates image
    status = MagickAnnotateImage (magick_wand, drawing_wand, x, y, angle,
            text);
    if (status == MagickFalse) {
        fprintf (stderr, "Couldn't annotate image\n");
        code = 1;
    }

    drawing_wand = DestroyDrawingWand (drawing_wand);
    pixel_wand = DestroyPixelWand (pixel_wand);

    return code;
}

/**
 * Annotates one line of 'text' of 'color' on the selected 'corner' of the
 * image on 'img_path' of size 'width'x'height'.
 */
int annotate_watermark (MagickWand *magick_wand, int width, int height,
        int corner, int pad, char *text, char *color) {

    int len = strlen (text);
    int x, y;

    switch (corner) {

        case top_left:
        default:
            x = pad;
            y = FONT_SIZE + pad;
            break;

        case top_right:
            x = width - len * FONT_WIDTH - pad;
            y = FONT_SIZE + pad;
            break;

        case bottom_right:
            x = width - len * FONT_WIDTH - pad;
            y = height - pad;
            break;

        case bottom_left:
            x = pad;
            y = height - pad;
            break;
    }

    return annotate_img (magick_wand, x, y, 0, text, color);
}

/**
 * Paints y axis values on 'img' from 'min' to 'max'. The color is 'color'.
 *
 *      - If the scale is linear, each line marks 'step' more than the
 *      previous one.
 *      - If the scale is logarithmic, each line marks 'y * step' more than
 *      the previous one.
 */
int annotate_axis_values (MagickWand *magick_wand, int width, int height,
        float min, float max, int scale, float step, char *color) {

    int code = 0;

    float scaled_min = apply_scale (scale, min);
    float scaled_max = apply_scale (scale, max);
    int j, j_text;
    float y;

    const int pad = 2;
    char text[10];

    y = min;
    while (y <= max) {

        j = height - (apply_scale (scale, y) - scaled_min) * height
            / (scaled_max - scaled_min);
        if (j == height)
            j--;

        if (y != (int) y)
            sprintf (text, "%.1f", y);
        else
            sprintf (text, "%.0f", y);

        j_text = j + FONT_SIZE;
        if (j_text > height)
            j_text = j - pad;

        code = annotate_img (magick_wand, pad, j_text, 0, text, color);
        if (code != 0)
            return code;

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

    return code;
}

