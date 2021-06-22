/**
 * My image processing library. Uses the MagickWand API.
 *
 * When using it always start by calling
 * 'start_img_proc (char *img_path, int width, int height, int pad)'
 * with the path, width, height and padding of the image that needs to be
 * processed. After making any changes with other functions, apply them by
 * calling 'apply_img_proc ()'. Always finish by calling
 * 'finish_img_proc ()'.
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

static MagickWand *Magick_Wand; // Global magick wand
static char *Img_Path; // Global image path
static int Width, Height, Pad; // Global width, height and padding
                               // of the image

/**
 * Returns the defined font size (height).
 */
int get_img_proc_font_size () {

    return FONT_SIZE;
}

/**
 * If its not instantiated, initializes the MagickWand environment, creates
 * the 'Magick_Wand' and saves it as a global variable. Then, reads
 * 'img_path' into 'Magick_Wand'. It also saves the 'img_path', 'width',
 * 'height' and 'pad' of the image.
 *
 * Must be the first function to run of this library!!!
 */
int start_img_proc (char *img_path, int width, int height, int pad) {

    Img_Path = img_path;
    Width = width;
    Height = height;
    Pad = pad;

    // If MagickWand environment hasn't been instantiated...
    if (IsMagickWandInstantiated() == MagickFalse) {
        // Initializes MagickWand environment and creates magick wand
        MagickWandGenesis ();
        Magick_Wand = NewMagickWand ();
    }

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
int annotate_img (double x, double y, double angle, char *text,
        char *color) {

    // If MagickWand environment hasn't been instantiated...
    if (IsMagickWandInstantiated() == MagickFalse) {
        fprintf (stderr,
                "MagickWand environment hasn't been instantiated.\n");
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
 * image.
 */
int annotate_watermark (int corner, char *text, char *color) {

    // If MagickWand environment hasn't been instantiated...
    if (IsMagickWandInstantiated() == MagickFalse) {
        fprintf (stderr,
                "MagickWand environment hasn't been instantiated.\n");
        return 1;
    }

    int len = strlen (text);
    int x, y;

    switch (corner) {

        case top_left:
        default:
            x = Pad;
            y = FONT_SIZE + Pad;
            break;

        case top_right:
            x = Width - Pad - len * FONT_WIDTH - FONT_WIDTH / 4.0;
            y = FONT_SIZE + Pad;
            break;

        case bottom_right:
            x = Width - Pad - len * FONT_WIDTH - FONT_WIDTH / 4.0;
            y = Height - Pad - FONT_SIZE / 4;
            break;

        case bottom_left:
            x = Pad;
            y = Height - Pad - FONT_SIZE / 4;
            break;
    }

    return annotate_img (x, y, 0, text, color);
}

/**
 * Paints x axis values on 'img' from 'min' to 'max' starting at 'first'
 * and spacing them by 'step'. The color is 'color'.
 *
 * Note that the values are painted on the bottom padding!!! If there is
 * not enought padding they won't show.
 */
int annotate_x_axis_values (float min, float max, float first, float step,
        char *color) {

    // If MagickWand environment hasn't been instantiated...
    if (IsMagickWandInstantiated() == MagickFalse) {
        fprintf (stderr,
                "MagickWand environment hasn't been instantiated.\n");
        return 1;
    }

    int code = 0;

    int i, i_text, j = Height - Pad + FONT_SIZE;

    char text[10];

    for (float x = first; x <= max; x += step) {

        i = Pad + (x - min) * (Width - 2 * Pad) / (max - min);

        if (i == Width - Pad)
            i--;

        if (x != (int) x)
            sprintf (text, "%.1f", x);
        else
            sprintf (text, "%.0f", x);

        i_text = i - strlen (text) / 2.0 * FONT_WIDTH; // Text is centered

        code = annotate_img (i_text, j, 0, text, color);
        if (code != 0)
            return code;
    }

    return code;
}

/**
 * Paints y axis values on 'img' from 'min' to 'max' starting at 'first'
 * and spacing them by 'step'. The color is 'color'.
 *
 *      - If the 'scale' is linear, each line is at 'y + step'.
 *      - If the 'scale' is logarithmic, each line is at 'y * step'.
 */
int annotate_y_axis_values (float min, float max, float first, int scale,
        float step, char *color) {

    // If MagickWand environment hasn't been instantiated...
    if (IsMagickWandInstantiated() == MagickFalse) {
        fprintf (stderr,
                "MagickWand environment hasn't been instantiated.\n");
        return 1;
    }

    int code = 0;

    float scaled_min = apply_scale (scale, min);
    float scaled_max = apply_scale (scale, max);
    int i = Pad, j, j_text;
    float y;

    char text[10];

    y = first;
    while (y <= max) {

        if (y != (int) y)
            sprintf (text, "%.1f", y);
        else
            sprintf (text, "%.0f", y);

        j = Height - Pad -
            (apply_scale (scale, y) - scaled_min) * (Height - 2 * Pad)
            / (scaled_max - scaled_min);

        if (j == Height - Pad)
            j--;

        j_text = j + FONT_SIZE;
        if (j_text > (Height - Pad))
            j_text = j - FONT_SIZE / 4;

        code = annotate_img (i, j_text, 0, text, color);
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
 * Uses 'Magick_Wand' to write image to 'Img_Path'.
 */
int apply_img_proc () {

    // If MagickWand environment hasn't been instantiated...
    if (IsMagickWandInstantiated() == MagickFalse) {
        fprintf (stderr,
                "MagickWand environment hasn't been instantiated.\n");
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

