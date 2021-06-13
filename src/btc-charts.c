/**
 * Generator of multiple Bitcoin charts PNG images using C + libpng.
 *
 * Javi Bonafonte
 *
 * TODO
 *  - use only MagickWand ?
 *  - add anotations (prices, dates, ...)
 *  - stock to flow
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <MagickWand/MagickWand.h>

#include "myimg/myimg.h"
#include "myimgproc/myimgproc.h"

#define STR_LEN 128

// Image constants
#define IMG_PATH "charts/chart.png"
#define WIDTH 1024
#define HEIGHT 576

// BTC CSV constants
#define BTC_CSV_PATH "data/bitcoinity_data.csv"

struct row {
    char date[STR_LEN];
    float price;
};

enum scales {linear, logarithmic};

/**
 * Returns number of lines in 'fp'.
 */
int get_num_lines (FILE *fp) {

    int num_lines = 0;
    char c;

    rewind (fp);

    while ((c = fgetc (fp)) != EOF) {
        if (c=='\n')
            num_lines++;
    }

    return num_lines;
}

/**
 * Returns average price of all the availble exchanges prices in one row of
 * the Bitcoin CSV file.
 *
 * (Specific to 'bitcoinity_data.csv')
 */
float get_row_avg_price (FILE *fp) {

    char price_str[STR_LEN];
    int c;
    float price, avg = 0;
    int count = 0;

    while ((c = fgetc (fp)) == ',') {
        fscanf (fp, "%[^,\n]", price_str);
        price = strtof (price_str, NULL);
        if (price > 0) {
            avg += price;
            count++;
        }
    }

    if (count > 0)
        return avg / count;
    else
        return -1;
}

/**
 * Saves all rows except the titles one from 'fp' in the 'rows' array.
 */
void get_rows (struct row *rows, FILE *fp) {

    rewind (fp);

    fscanf (fp, "%*[^\n]\n"); // Reads first line (titles line)

    // Reads rest of lines and saves date and average price in 'rows'
    // (Specific for 'bitcoinity_data.csv')
    for (int i = 0; fscanf (fp, "%[^,]", rows[i].date) != EOF; i++)
        rows[i].price = get_row_avg_price (fp);
}

/**
 * Returns the maximum value of 'rows.price'.
 */
int get_max_price (struct row *rows, int num_rows) {

    int max_price = 0;

    for (int i = 0; i < num_rows; i++) {
        if (rows[i].price > max_price)
            max_price = rows[i].price;
    }

    return max_price;
}

/**
 * Applies 'scale' to 'value' and returns it.
 */
float apply_scale (int scale, float value) {

    switch (scale) {

        case linear:
            return value;

        case logarithmic:
            if (value <= 0)
                return value;
            return (float) log (value);

        default:
            return -1;
    }
}

/**
 *
 */
void paint_rainbow_column (int *img, int x, int j, int thickness,
        int min_hue, int max_hue, int alpha) {

    int r, g, b;
    float hue;

    for (int y = 0; y < HEIGHT; y++) {
        if (abs (y - j) < thickness) {

            hue = ((y - j + thickness + min_hue) * max_hue / thickness)
                % 360;
            hsl_to_rgb (&r, &g, &b, hue, 1, 0.7);
            set_rgba (img, WIDTH, x, y, r, g, b, alpha);
        }
    }
}

/**
 *
 */
void paint_function_column (int *img, int x, int j, int prev_j,
        int r, int g, int b, int a) {

    for (int y = 0; y < HEIGHT; y++) {
        if ((x > 0 && ((j < prev_j && y < prev_j && y > j)
                        || (j > prev_j && y > prev_j && y < j))) 
                || y == j) {
            set_rgba (img, WIDTH, x, y, r, g, b, a);
        }
    }
}

/**
 *
 */
void paint_trololo (int *img, float min_x, float max_x, float min_y,
        float max_y, int y_scale) {

    min_y = apply_scale (y_scale, min_y);
    max_y = apply_scale (y_scale, max_y);
    int day, j, prev_j;
    float value;


    for (int x = 0; x < WIDTH; x++) {

        day = (int) x * (max_x - min_x) / WIDTH + min_x;

        //value = apply_scale (linear, 0.2911 * x); // y = 0.2911x
        
        // y = 10^(2.9065 * ln (days from 2009/01/09) - 19.493)
        value = apply_scale (y_scale,
                (float) pow (10, 2.9065 * log (day) - 19.493));

        j = HEIGHT - (int) ((value - min_y) * HEIGHT) / (max_y - min_y);

        paint_rainbow_column (img, x, j, 80, 0, 120, 255);
        //paint_function_column (img, x, j, prev_j, 255, 0, 0, 255);

        prev_j = j;
    }
}

/**
 *
 */
void paint_price (int *img, struct row *rows, int num_rows, float min,
        float max, int y_scale) {

    min = apply_scale (y_scale, min);
    max = apply_scale (y_scale, max);
    int x, j, prev_j;
    double price;

    for (x = 0; x < WIDTH; x++) {

        price = apply_scale (y_scale,
                rows[x * (int) num_rows / WIDTH].price);

        if (price != -1) {

            j = HEIGHT - (int) ((price - min) * HEIGHT) / (max - min);

            paint_function_column (img, x, j, prev_j, 0, 0, 0, 255);

            prev_j = j;
        }
    }
}

/**
 * Generates desired chart image using 'libpng' (functions in 'myimg.h').
 *
 * Uses some data for the generation passed as arguments.
 */
int generate_img (struct row *rows, int num_rows) {

    int code = 0;

    // Creates 'img' buffer
    int *img = create_img (WIDTH, HEIGHT);
    if (img == NULL) {
        fprintf (stderr, "Couldn't create image buffer\n");
        return 1;
    }

    // Paints data to 'img' buffer

    paint_img_background (img, WIDTH, HEIGHT, 0, 0, 0, 0);

    //paint_price (img, rows, num_rows, 0, 65000, linear);

    paint_trololo (img, 554, num_rows + 554, 0.1, 1000000, logarithmic);
    paint_price (img, rows, num_rows, 0.1, 1000000, logarithmic);

    // Writes 'img' buffer to file
    code = write_img (img, IMG_PATH, WIDTH, HEIGHT);

    // Frees 'img' buffer
    if (img != NULL)
        free (img);

    return code;
}

/**
 * Processes image using 'MagickWand' lib (fucntions in 'myimgproc.h').
 */
int process_img () {

    int code = 0;

    MagickWand *magick_wand;

    MagickWandGenesis (); // Initializaes MagickWand environment

    magick_wand = NewMagickWand ();

    // Reads image
    code = magick_read_img (magick_wand, IMG_PATH);
    if (code != 0)
        goto finalise;

    // Paints log axis
    code = paint_log_axis (magick_wand, WIDTH, HEIGHT,
            0.1, 1000000, "rgb(128, 128, 128)");
    if (code != 0)
        goto finalise;

    // Annotates watermark
    code = annotate_watermark (magick_wand, WIDTH, HEIGHT, bottom_right, 4,
            "javibonafonte.com", "rgb(128, 128, 128)");
    if (code != 0)
        goto finalise;

    // Writes image
    code = magick_write_img (magick_wand, IMG_PATH);
    if (code != 0)
        goto finalise;

finalise:

    magick_wand = DestroyMagickWand (magick_wand);

    MagickWandTerminus(); // Terminates MagickWand environment
    
    return code;
}

/**
 * Main.
 *
 *      1. Gets the needed data from the CSV file.
 *      2. Generates image using that data.
 *      3. Processes generated image using that data.
 */
int main (int argc, char *argv[]) {

    int code = 0;

    // Opens BTC csv file for reading
    FILE *fp = fopen (BTC_CSV_PATH, "r");
    if (fp == NULL) {
        fprintf (stderr, "Couldn't open '%s' file\n", BTC_CSV_PATH);
        return 1;
    }

    // Gets data from BTC CSV file
    int num_rows = get_num_lines (fp) - 1;
    struct row rows[num_rows];
    get_rows (rows, fp);

    fclose (fp);

    //float max_price = get_max_price (rows, num_rows);

    // Generates image
    code = generate_img (rows, num_rows);
    if (code != 0)
        goto finalise;

    // Processes image
    code = process_img ();
    if (code != 0)
        goto finalise;

finalise:

    return code;
}

