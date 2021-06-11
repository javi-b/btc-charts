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
 * Annotates 'text' of 'color' on 'image_path' on position 'x','y' and
 * 'angle'.
 *
 * 'x' on the left of the text.
 * 'y' is the baseline of the text.
 */
int annotate_img (char *img_path, double x, double y, double angle,
        char *text, char *color) {

    int code = 0;

    MagickBooleanType status;
    MagickWand *magick_wand;
    DrawingWand *drawing_wand;
    PixelWand *pixel_wand;

    MagickWandGenesis (); // Initializaes MagickWand environment

    magick_wand = NewMagickWand ();
    drawing_wand = NewDrawingWand ();
    pixel_wand = NewPixelWand ();

    // Sets drawing wand
    DrawSetTextAntialias (drawing_wand, 0);
    // (list available fonts in your system running 'convert -list font')
    DrawSetFont (drawing_wand, FONT);
    DrawSetFontSize (drawing_wand, FONT_SIZE);
    PixelSetColor (pixel_wand, color);
    DrawSetFillColor (drawing_wand, pixel_wand);

    // Reads image
    status = MagickReadImage (magick_wand, img_path);
    if (status == MagickFalse) {
        fprintf (stderr, "Couldn't read image '%s'\n", img_path);
        code = 1;
        goto finalise;
    }

    // Annotates image
    status = MagickAnnotateImage (magick_wand, drawing_wand, x, y, angle,
            text);
    if (status == MagickFalse) {
        fprintf (stderr, "Couldn't annotate image\n");
        code = 1;
        goto finalise;
    }

    // Writes image
    status = MagickWriteImage (magick_wand, img_path);
    if (status == MagickFalse) {
        fprintf (stderr, "Couldn't write image '%s'\n", img_path);
        code = 1;
        goto finalise;
    }

finalise:

    magick_wand = DestroyMagickWand (magick_wand);
    drawing_wand = DestroyDrawingWand (drawing_wand);
    pixel_wand = DestroyPixelWand (pixel_wand);

    MagickWandTerminus(); // Terminates MagickWand environment

    return code;
}

/**
 * Annotates one line of 'text' of 'color' on the selected 'corner' of the
 * image on 'img_path' of size 'width'x'height'.
 */
int paint_watermark (char *img_path, int width, int height, int corner,
        char *text, char *color) {

    const int pad = 2;
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

    return annotate_img (img_path, x, y, 0, text, color);
}

