/**
 * My image processing library.
 *
 * Javi Bonafonte
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MagickWand/MagickWand.h>

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
 * Draws line of 'color' from 'sx','sy' to 'ex','ey' to image in
 * 'magick_wand'.
 */
int draw_line (MagickWand *magick_wand, double sx, double sy,
        double ex, double ey, char *color) {

    int code = 0;
    MagickBooleanType status;

    DrawingWand *drawing_wand;
    PixelWand *pixel_wand;

    drawing_wand = NewDrawingWand ();
    pixel_wand = NewPixelWand ();

    // Sets drawing wand
    PixelSetColor (pixel_wand, color);
    DrawSetStrokeColor (drawing_wand, pixel_wand);

    // Draws line
    DrawLine (drawing_wand, sx, sy, ex, ey);
    status = MagickDrawImage (magick_wand, drawing_wand);
    if (status == MagickFalse) {
        fprintf (stderr, "Couldn't draw line\n");
        code = 1;
    }

    drawing_wand = DestroyDrawingWand (drawing_wand);
    pixel_wand = DestroyPixelWand (pixel_wand);

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
 * Paints logarithmic scale y axis lines on 'img' from 'min' to 'max'.
 * Each line marks '10 * previous line'.
 */
int paint_log_axis (MagickWand *magick_wand, int width, int height,
        float min, float max, char *color) {

    int code = 0;
    float log_min = log (min);
    float log_max = log (max);
    int j, j_text;

    const int pad = 2;
    char text[10];

    for (float y = min; y <= max; y *= 10) {

        j = height - (log (y) - log_min) * height / (log_max - log_min);

        code = draw_line (magick_wand, 0, j, width - 1, j, color);
        if (code != 0)
            return code;

        sprintf (text, "%.0f", y);
        j_text = j + FONT_SIZE;
        if (j_text > height)
            j_text = j - pad;

        code = annotate_img (magick_wand, pad, j_text, 0, text, color);
        if (code != 0)
            return code;
    }

    return code;
}

