/**
 * Generator of multiple Bitcoin charts PNG images.
 *
 * Javi Bonafonte
 *
 * TODO
 *  - separate chart code from csv file code
 *  - when skipping days of price, paint the average of skipped days
 *  - use bitcoinity data for stock to flow
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "util/util.h"
#include "myimg/myimg.h"
#include "myimgproc/myimgproc.h"

#define STR_LEN 128

// Image constants
#define IMG_PATH "charts/chart.png"
#define WIDTH 1024
#define HEIGHT 576

// BTC CSV constants
#define BTC_CSV_PATH "data/bitcoinity_data.csv"
#define DAYS_FROM_GEN 554 // Days from Genesis Block to first day in file
                          // Genesis Block was on 2009-01-09

// Struct with BTC price for a date (represents one row of the CSV file)
struct row {
    char date[STR_LEN];
    float price;
};

// Struct with chart configuration parameters
struct chart_cfg {
    int w, h; // width and height of chart image
    int paint_price, paint_trololo, paint_sf_model; // things to paint (0/1)
    float min_x, max_x, min_y, max_y; // x values and y values to paint
                                      // x = days, y = price
    int scale; // scale of the y axis (linear or logarithmic)
    float axis_step; // step of change between y axis lines
};

// ------------------------------------------------------------------------

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

// ------------------------------------------------------------------------

/**
 * Generates and returns the chart configuration.
 */
struct chart_cfg get_chart_cfg (int num_days_in_file, int scale,
        int paint_price, int paint_trololo, int paint_sf_model,
        int days_to_predict) {

    struct chart_cfg cfg;

    cfg.w = WIDTH;
    cfg.h = HEIGHT;

    cfg.paint_price = paint_price;
    cfg.paint_trololo = paint_trololo;
    cfg.paint_sf_model = paint_sf_model;

    cfg.scale = scale;

    switch (scale) {

        case linear:
            cfg.min_y = 0;
            cfg.max_y = 65000;
            cfg.axis_step = 10000;
            break;

        case logarithmic:
            cfg.min_y = 0.1;
            cfg.max_y = 10000000;
            cfg.axis_step = 10;
            break;
    }

    cfg.min_x = DAYS_FROM_GEN;
    cfg.max_x = DAYS_FROM_GEN + num_days_in_file + days_to_predict;

    return cfg;
}

// ------------------------------------------------------------------------

/**
 *
 */
int paint_rainbow_column (int x, int j, int thickness,
        int min_hue, int max_hue, int alpha) {

    int code = 0;

    int r, g, b;
    float hue;

    for (int y = 0; y < HEIGHT; y++) {
        if (abs (y - j) < thickness) {

            hue = ((y - j + thickness + min_hue) * max_hue / thickness)
                % 360;
            hsl_to_rgb (&r, &g, &b, hue, 1, 0.7);

            code = set_rgba (x, y, r, g, b, alpha);
            if (code != 0)
                return code;
        }
    }

    return code;
}

/**
 *
 */
int paint_function_column (int x, int j, int prev_j,
        int r, int g, int b, int a) {

    int code = 0;

    for (int y = 0; y < HEIGHT; y++) {
        if ((x > 0 && ((j < prev_j && y < prev_j && y > j)
                        || (j > prev_j && y > prev_j && y < j))) 
                || y == j) {

            code = set_rgba (x, y, r, g, b, a);
            if (code != 0)
                return code;
        }
    }

    return code;
}

/**
 *
 */
int paint_trololo (struct chart_cfg cfg) {

    int code = 0;

    cfg.min_y = apply_scale (cfg.scale, cfg.min_y);
    cfg.max_y = apply_scale (cfg.scale, cfg.max_y);
    int day, j, prev_j;
    float value;


    for (int x = 0; x < cfg.w; x++) {

        day = (int) x * (cfg.max_x - cfg.min_x) / cfg.w + cfg.min_x;

        // y = 10^(2.9065 * ln (days from genesis) - 19.493)
        value = apply_scale (cfg.scale,
                (float) pow (10, 2.9065 * log (day) - 19.493));

        j = cfg.h - (int) ((value - cfg.min_y) * cfg.h)
            / (cfg.max_y - cfg.min_y);

        code = paint_rainbow_column (x, j, 80, 0, 120, 255);
        //code = paint_function_column (x, j, prev_j, 255, 0, 0, 255);
        if (code != 0)
            return code;

        prev_j = j;
    }

    return code;
}

/**
 * Returns Bitcoin block reward at the time of 'days_since_gen'.
 * (Aproximated theorical calculation).
 */
float get_btc_block_reward (int days_since_gen) {

    float reward = 50;
    int halvings = (int) (days_since_gen / 1460);

    for (int i = 0; i < halvings; i++)
        reward /= 2;

    return reward;
}

/**
 * Returns total stock of Bitcoin at the time of 'days_since_gen'.
 * (Aproximated theorical calculation).
 */
float get_btc_stock (int days_since_gen) {

    float reward = 50;
    int halvings = (int) (days_since_gen / 1460);
    float stock;

    for (int i = 0; i < halvings; i++) {
        stock += 210000 * reward;
        reward /= 2;
    }

    int days_since_last_halving = days_since_gen % 1460;
    // One block per every 10 minutes of the day (144 blocks per day)
    int blocks_since_last_halving = days_since_last_halving * 144;

    stock += blocks_since_last_halving * reward;

    return stock;
}

/**
 *
 */
int paint_sf_model (struct chart_cfg cfg) {

    int code = 0;

    cfg.min_y = apply_scale (cfg.scale, cfg.min_y);
    cfg.max_y = apply_scale (cfg.scale, cfg.max_y);
    int day, j, prev_j;
    float stock, reward, sf, model;

    for (int x = 0; x < cfg.w; x++) {

        day = (int) x * (cfg.max_x - cfg.min_x) / cfg.w + cfg.min_x;
        stock = get_btc_stock (day);
        reward = get_btc_block_reward (day);
        sf = stock / (reward * 365 * 144);
        model = apply_scale (cfg.scale, exp (-1.84) * pow (sf, 3.36));

        j = cfg.h - (int) ((model - cfg.min_y) * cfg.h)
            / (cfg.max_y - cfg.min_y);

        code = paint_function_column (x, j, prev_j, 255, 0, 0, 255);
        if (code != 0)
            return code;

        prev_j = j;
    }

    return code;
}

/**
 *
 */
int paint_price (struct row *rows, int num_rows,
        struct chart_cfg cfg) {

    int code = 0;

    cfg.min_y = apply_scale (cfg.scale, cfg.min_y);
    cfg.max_y = apply_scale (cfg.scale, cfg.max_y);
    int row, x, j, prev_j;
    double price;

    for (x = 0; x < cfg.w; x++) {

        row = (int) x * (cfg.max_x - cfg.min_x)
            / cfg.w + cfg.min_x - DAYS_FROM_GEN;

        if (row < num_rows)
            price = apply_scale (cfg.scale, rows[row].price);
        else
            price = -1;

        if (price != -1) {

            j = cfg.h- (int) ((price - cfg.min_y) * cfg.h)
                / (cfg.max_y - cfg.min_y);

            code = paint_function_column (x, j, prev_j, 0, 0, 0, 255);
            if (code != 0)
                return code;

            prev_j = j;
        }
    }

    return code;
}

/**
 * Generates desired chart image using 'myimg' library.
 *
 * Uses some data passed as arguments.
 */
int generate_img (struct row *rows, int num_rows, struct chart_cfg cfg) {

    int code = 0;

    // Creates image
    //code = create_img (cfg.w, cfg.h);
    if (code != 0)
        goto finalise;

    // Paints image background
    code = paint_img_background (0, 0, 0, 0);
    if (code != 0)
        goto finalise;

    if (cfg.paint_trololo) {
        // Paints trololo
        code = paint_trololo (cfg);
        if (code != 0)
            goto finalise;
    }

    // Paints axis
    code = paint_axis (cfg.min_y, cfg.max_y, cfg.scale, cfg.axis_step,
            204, 204, 204, 255);
    if (code != 0)
        goto finalise;

    if (cfg.paint_sf_model) {
        // Paints stock to flow model
        code = paint_sf_model (cfg);
        if (code != 0)
            goto finalise;
    }

    if (cfg.paint_price) {
        // Paints price
        code = paint_price (rows, num_rows, cfg);
        if (code != 0)
            goto finalise;
    }

    // Writes image to file
    code = write_img (IMG_PATH);

finalise:

    // Frees image
    free_img ();
    return code;
}

/**
 * Processes image using 'myimgproc' library.
 */
int process_img (struct chart_cfg cfg) {

    int code = 0;

    // Starts image processing of 'IMG_PATH'
    code = start_img_proc (IMG_PATH);
    if (code != 0)
        goto finalise;

    // Annotates axis
    code = annotate_axis_values (cfg.w, cfg.h, cfg.min_y, cfg.max_y,
            cfg.scale, cfg.axis_step, "rgb(128, 128, 128)");
    if (code != 0)
        goto finalise;

    // Annotates watermark
    code = annotate_watermark (cfg.w, cfg.h, bottom_right, 4,
            "javibonafonte.com", "rgb(128, 128, 128)");
    if (code != 0)
        goto finalise;

    // Applies the changes
    code = apply_img_proc ();

finalise:

    // Finishes image processing
    finish_img_proc ();
    return code;
}

// ------------------------------------------------------------------------

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

    struct chart_cfg cfg = get_chart_cfg (num_rows,
            logarithmic, 1, 1, 1, 0);

    // Generates image
    code = generate_img (rows, num_rows, cfg);
    if (code != 0)
        goto finalise;

    // Processes image
    code = process_img (cfg);
    if (code != 0)
        goto finalise;

finalise:

    return code;
}

