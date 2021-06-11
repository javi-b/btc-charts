/**
 * My image processing library.
 *
 * Javi Bonafonte
 */

#include <stdlib.h>
#include <stdio.h>
#include <MagickWand/MagickWand.h>

#include "myimgproc.h"

#define FONT "Unifont" // Font family used to annotate
#define FONT_SIZE 13 // Font size used to annotate

/**
 * Annotates 'text' on 'image_path' on position 'x','y' and 'angle'.
 */
int annotate_img (char *img_path, double x, double y, double angle,
        char *text) {

    int code = 0;

    MagickBooleanType status;
    MagickWand *magick_wand;
    DrawingWand *drawing_wand;

    MagickWandGenesis (); // Initializaes MagickWand environment

    magick_wand = NewMagickWand ();
    drawing_wand = NewDrawingWand ();

    // Sets drawing wand
    DrawSetTextAntialias (drawing_wand, 0);
    // (list available fonts in your system running 'convert -list font')
    DrawSetFont (drawing_wand, FONT);
    DrawSetFontSize (drawing_wand, FONT_SIZE);

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

    MagickWandTerminus(); // Terminates MagickWand environment

    return code;
}
