/**
 * TODO
 *  - add grid
 *  - add anotations (prices, date, javibonafonte.com, ...)
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "myimg.h"

#define STR_LEN 128

// Image constants
#define IMG_PATH "chart.png"
#define WIDTH 1024
#define HEIGHT 576

// BTC CSV constants
#define BTC_CSV_PATH "bitcoinity_data.csv"

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
void paint_function (int *image, float min_x, float max_x, float min_y,
        float max_y, int y_scale) {

    min_y = apply_scale (y_scale, min_y);
    max_y = apply_scale (y_scale, max_y);
    int x, y, day, j, prev_j;
    float value;

    for (x = 0; x < WIDTH; x++) {

        day = (int) x * (max_x - min_x) / WIDTH + min_x;

        //value = apply_scale (linear, 0.2911 * x); // y = 0.2911x
        
        // y = 10^(2.9065 * ln (days from 2009/01/09) - 19.493)
        value = apply_scale (y_scale,
                (float) pow (10, 2.9065 * log (day) - 19.493));

        j = HEIGHT - (int) ((value - min_y) * HEIGHT) / (max_y - min_y);

        for (y = 0; y < HEIGHT; y++) {
            if ((x > 0 && ((j < prev_j && y < prev_j && y > j)
                            || (j > prev_j && y > prev_j && y < j))) 
                    || y == j) {
                set_rgba (image, WIDTH, x, y, 255, 0, 0, 255);
            }
        }
        prev_j = j;
    }
}

/**
 *
 */
void paint_price_image (int *image, struct row *rows, int num_rows,
        float min, float max, int scale) {

    min = apply_scale (scale, min);
    max = apply_scale (scale, max);
    int x, y, j, prev_j;
    double price;

    for (x = 0; x < WIDTH; x++) {

        price = apply_scale (scale,
                rows[x * (int) num_rows / WIDTH].price);

        if (price != -1) {

            j = HEIGHT - (int) ((price - min) * HEIGHT) / (max - min);

            for (y = 0; y < HEIGHT; y++) {
                if ((x > 0 && ((j < prev_j && y < prev_j && y > j)
                                || (j > prev_j && y > prev_j && y < j)))
                        || y == j) {
                    set_rgba (image, WIDTH, x, y, 0, 0, 0, 255);
                }
            }
            prev_j = j;
        }
    }
}

/**
 * Main.
 */
int main (int argc, char *argv[]) {

    // Opens BTC csv file for reading
    FILE *fp = fopen (BTC_CSV_PATH, "r");
    if (fp == NULL) {
        fprintf (stderr, "Couldn't open '%s' file\n", BTC_CSV_PATH);
        return 1;
    }

    // Gets data from BTC csv file
    int num_rows = get_num_lines (fp) - 1;
    struct row rows[num_rows];
    get_rows (rows, fp);

    fclose (fp);

    //float max_price = get_max_price (rows, num_rows);

    // Creates 'image' buffer
    int *image = create_image (WIDTH, HEIGHT);
    if (image == NULL) {
        fprintf (stderr, "Couldn't create image buffer\n");
        return 1;
    }

    // Paints data to 'image' buffer
    paint_image_background (image, WIDTH, HEIGHT, 0, 0, 0, 0);

    //paint_price_image (image, rows, num_rows, 0, 65000, linear);

    paint_function (image, 554, num_rows + 554, 0.1, 100000, logarithmic);
    paint_price_image (image, rows, num_rows, 0.1, 100000, logarithmic);

    // Writes 'image' buffer to file
    return write_image (image, IMG_PATH, WIDTH, HEIGHT);
}

