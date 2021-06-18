/**
 * My image processing library. Uses the MagickWand API.
 *
 * When using it always start by calling 'start_img_proc (char *img_path)'
 * with the path of the image that needs to be processed. After making any
 * changes with other functions, apply them by calling 'apply_img_proc ()'.
 * Always finish by calling 'finish_img_proc ()'.
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

MagickWand *Magick_Wand; // Global magick wand
char *Img_Path; // Global image path

/**
 * If its not instantiated, initializes the MagickWand environment and
 * creates the 'Magick_Wand'. Then, reads 'img_path' into 'Magick_Wand'.
 *
 * Must be the first function to run of this library!!!
 */
int start_img_proc (char *img_path) {

    // If MagickWand environment is not instantiated...
    if (IsMagickWandInstantiated() == MagickFalse) {
        // Initializes MagickWand environment and creates magick wand
        MagickWandGenesis ();
        Magick_Wand = NewMagickWand ();
    }

    Img_Path = img_path;

    MagickBooleanType status;

    // Reads image
    status = MagickReadImage (Magick_Wand, Img_Path);
    if (status == MagickFalse) {
        fprintf (stderr, "Couldn't read image '%s'\n", Img_Path);
        return 1;
    }

    return 0;
}

/**
 * Annotates 'text' of 'color' on image in 'Magick_Wand' on position
 * 'x','y' and 'angle'.
 *
 * 'x' on the left of the text.
 * 'y' is the baseline of the text.
 */
int annotate_img (double x, double y, double angle,
        char *text, char *color) {

    // If MagickWand environment is not instantiated...
    if (IsMagickWandInstantiated() == MagickFalse) {
        fprintf (stderr, "MagickWand environment is not instantiated.\n");
        return 1;
    }

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
    status = MagickAnnotateImage (Magick_Wand, drawing_wand, x, y, angle,
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
 * image of size 'width'x'height'.
 */
int annotate_watermark (int width, int height,
        int corner, int pad, char *text, char *color) {

    // If MagickWand environment is not instantiated...
    if (IsMagickWandInstantiated() == MagickFalse) {
        fprintf (stderr, "MagickWand environment is not instantiated.\n");
        return 1;
    }

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

    return annotate_img (x, y, 0, text, color);
}

/**
 * Paints y axis values on 'img' from 'min' to 'max'. The color is 'color'.
 *
 *      - If the 'scale' is linear, each line is at 'y + step'.
 *      - If the 'scale' is logarithmic, each line is at 'y * step'.
 */
int annotate_axis_values (int width, int height, float min, float max,
        int scale, float step, char *color) {

    // If MagickWand environment is not instantiated...
    if (IsMagickWandInstantiated() == MagickFalse) {
        fprintf (stderr, "MagickWand environment is not instantiated.\n");
        return 1;
    }

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

        code = annotate_img (pad, j_text, 0, text, color);
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

/**
 * Uses 'Magick_Wand' to write image to 'Img_Path.
 */
int apply_img_proc () {

    // If MagickWand environment is not instantiated...
    if (IsMagickWandInstantiated() == MagickFalse) {
        fprintf (stderr, "MagickWand environment is not instantiated.\n");
        return 1;
    }

    MagickBooleanType status;

    // Writes image
    status = MagickWriteImage (Magick_Wand, Img_Path);
    if (status == MagickFalse) {
        fprintf (stderr, "Couldn't write image '%s'\n", Img_Path);
        return 1;
    }

    return 0;
}

/**
 * Tries to destroy the 'Magick_Wand' and terminate the environment.
 *
 * Must be the last function to run of this library!!!
 */
void finish_img_proc () {

    if (Magick_Wand != NULL)
        Magick_Wand = DestroyMagickWand (Magick_Wand);

    if (IsMagickWandInstantiated() == MagickTrue)
        MagickWandTerminus();
}

