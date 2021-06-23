/**
 * Generator of multiple Bitcoin charts PNG images.
 *
 * Javi Bonafonte
 *
 * TODO
 *      - rgba is in a color struct
 *      - use bitcoinity data for stock to flow
 *      - mark halvings ?
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "util/util.h"
#include "btc/btc.h"
#include "btcdata/btcdata.h"
#include "img/img.h"
#include "imgproc/imgproc.h"

// Image constants
#define IMG_PATH "charts/chart.png"
#define WIDTH 1024
#define HEIGHT 576

// Struct containing the chart image configuration parameters
struct chart_cfg {
    int w, h, pad; // width, height and padding of chart image
    int paint_price, paint_trololo, paint_sf_model; // things to paint (0/1)

    // x values to paint (days)
    float min_x, max_x, first_x_axis, x_axis_step;
    // x values in years
    float min_x_year, max_x_year, first_x_year, x_axis_step_year;
    // y values to paint (dollars)
    float min_y, max_y, first_y_axis, y_axis_step;

    int scale; // scale of the y axis (linear or logarithmic)
};

/**
 * Generates and returns the chart image configuration.
 */
struct chart_cfg get_chart_cfg (int scale,
        int paint_price, int paint_trololo, int paint_sf_model,
        int days_to_predict) {

    struct chart_cfg cfg;

    cfg.w = WIDTH;
    cfg.h = HEIGHT;
    cfg.pad = get_img_proc_font_size () + 2;

    cfg.paint_price = paint_price;
    cfg.paint_trololo = paint_trololo;
    cfg.paint_sf_model = paint_sf_model;

    cfg.scale = scale;

    switch (scale) {

        case linear:
            cfg.min_y = 0;
            cfg.max_y = 65000;
            cfg.first_y_axis = 10000;
            cfg.y_axis_step = 10000;
            break;

        case logarithmic:
            cfg.min_y = 0.1 * 0.9;
            cfg.max_y = 1000000 * 1.1;
            cfg.first_y_axis = 0.1;
            cfg.y_axis_step = 10;
            break;
    }

    cfg.min_x = get_num_days_from_gen_to_first_btc_data ();
    cfg.max_x = get_num_days_from_gen_to_first_btc_data ()
        + get_num_btc_data_days () + days_to_predict;
    cfg.x_axis_step = 2 * 365;

    cfg.min_x_year = days_since_gen_to_years (cfg.min_x);
    cfg.max_x_year = days_since_gen_to_years (cfg.max_x);
    cfg.first_x_year = ceil (cfg.min_x_year);
    cfg.x_axis_step_year = cfg.x_axis_step / 365;

    return cfg;
}

/**
 *
 */
int paint_rainbow_column (struct chart_cfg cfg, int x, int j,
        int thickness, int min_hue, int max_hue, int alpha) {

    int code = 0;

    int r, g, b;
    float hue;

    for (int y = cfg.pad; y < cfg.h - cfg.pad; y++) {
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
int paint_function_column (struct chart_cfg cfg, int x, int j, int prev_j,
        int r, int g, int b, int a) {

    int code = 0;

    for (int y = cfg.pad; y < cfg.h - cfg.pad; y++) {
        if ((prev_j != -1 && ((j < prev_j && y < prev_j && y > j)
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
    int day, j, prev_j = -1;
    float value;


    for (int x = cfg.pad; x < cfg.w - cfg.pad; x++) {

        day = cfg.min_x +
            (int) (x - cfg.pad) * (cfg.max_x - cfg.min_x)
            / (cfg.w - 2 * cfg.pad);

        // y = 10^(2.9065 * ln (days from genesis) - 19.493)
        value = apply_scale (cfg.scale,
                (float) pow (10, 2.9065 * log (day) - 19.493));

        j = cfg.h - cfg.pad -
            (int) ((value - cfg.min_y) * (cfg.h - 2 * cfg.pad))
            / (cfg.max_y - cfg.min_y);

        code = paint_rainbow_column (cfg, x, j, 70, 0, 120, 255);
        //code = paint_function_column (cfg, x, j, prev_j, 255, 0, 0, 255);
        if (code != 0)
            return code;

        prev_j = j;
    }

    return code;
}

/**
 *
 */
int paint_sf_model (struct chart_cfg cfg) {

    int code = 0;

    cfg.min_y = apply_scale (cfg.scale, cfg.min_y);
    cfg.max_y = apply_scale (cfg.scale, cfg.max_y);
    int day, j, prev_j = -1;
    float stock, reward, sf, model;

    for (int x = cfg.pad; x < cfg.w - cfg.pad; x++) {

        day = cfg.min_x +
            (int) (x - cfg.pad) * (cfg.max_x - cfg.min_x)
            / (cfg.w - 2 * cfg.pad);
        stock = get_btc_stock (day);
        reward = get_btc_block_reward (day);
        sf = stock / (reward * 365 * 144);
        model = apply_scale (cfg.scale, exp (-1.84) * pow (sf, 3.36));

        j = cfg.h - cfg.pad -
            (int) ((model - cfg.min_y) * (cfg.h - 2 * cfg.pad))
            / (cfg.max_y - cfg.min_y);

        code = paint_function_column (cfg, x, j, prev_j, 255, 0, 0, 255);
        if (code != 0)
            return code;

        prev_j = j;
    }

    return code;
}

/**
 *
 */
int paint_price (struct chart_cfg cfg) {

    int code = 0;

    cfg.min_y = apply_scale (cfg.scale, cfg.min_y);
    cfg.max_y = apply_scale (cfg.scale, cfg.max_y);
    int num_days = get_num_btc_data_days ();
    int day, prev_day = -1, x, j, prev_j = -1;
    double price;

    for (x = cfg.pad; x < cfg.w - cfg.pad; x++) {

        day = (int) (x - cfg.pad) * (cfg.max_x - cfg.min_x)
            / (cfg.w - 2 * cfg.pad);

        if (day < num_days)
            price = apply_scale (cfg.scale,
                    get_avg_price (prev_day, day));
        else
            price = -1;

        if (price != -1) {

            j = cfg.h - cfg.pad -
                (int) ((price - cfg.min_y) * (cfg.h - 2 * cfg.pad))
                / (cfg.max_y - cfg.min_y);

            code = paint_function_column (cfg, x, j, prev_j, 0, 0, 0, 255);
            if (code != 0)
                return code;

            prev_j = j;
        }

        prev_day = day;
    }

    return code;
}

/**
 * Generates desired chart image using 'img' library.
 *
 * Uses some data passed as arguments.
 */
int generate_img (struct chart_cfg cfg) {

    int code = 0;

    // Creates image
    code = create_img (cfg.w, cfg.h, cfg.pad);
    if (code != 0)
        goto finalise;

    // Paints image background
    code = paint_img_background (0, 0, 0, 0);
    if (code != 0)
        goto finalise;

    // Paints image border
    code = paint_img_border (128, 128, 128, 255);
    if (code != 0)
        goto finalise;

    if (cfg.paint_trololo) {
        // Paints trololo
        code = paint_trololo (cfg);
        if (code != 0)
            goto finalise;
    }

    // Paints x axis
    code = paint_x_axis (cfg.min_x_year, cfg.max_x_year, cfg.first_x_year,
            cfg.x_axis_step_year, 204, 204, 204, 255);
    if (code != 0)
        goto finalise;

    // Paints y axis
    code = paint_y_axis (cfg.min_y, cfg.max_y, cfg.first_y_axis,
            cfg.y_axis_step, cfg.scale, 204, 204, 204, 255);
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
        code = paint_price (cfg);
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
 * Processes image using 'imgproc' library.
 */
int process_img (struct chart_cfg cfg) {

    int code = 0;

    // Starts image processing
    code = start_img_proc (IMG_PATH, cfg.w, cfg.h, cfg.pad);
    if (code != 0)
        goto finalise;

    // Annotates x axis
    code =  annotate_x_axis_values (cfg.min_x_year, cfg.max_x_year,
            cfg.first_x_year, cfg.x_axis_step_year, "rgb(128, 128, 128)");
    if (code != 0)
        goto finalise;

    // Annotates y axis
    code = annotate_y_axis_values (cfg.min_y, cfg.max_y, cfg.first_y_axis,
            cfg.y_axis_step, cfg.scale, "rgb(128, 128, 128)");
    if (code != 0)
        goto finalise;

    // Annotates watermark
    code = annotate_watermark (bottom_right, "javibonafonte.com",
            "rgb(128, 128, 128)");
    if (code != 0)
        goto finalise;

    // Applies the changes
    code = apply_img_proc ();

finalise:

    // Finishes image processing
    finish_img_proc ();
    return code;
}

/**
 * Main.
 *
 *      1. Reads the Bitcoin data.
 *      2. Sets the chart image configuration.
 *      2. Generates image using that data and configuration.
 *      3. Processes generated image using that data and configuration.
 */
int main (int argc, char *argv[]) {

    int code = 0;

    // Reads Bitcoin data
    code = read_btc_data ();
    if (code != 0)
        goto finalise;

    // Sets chart image configuration
    struct chart_cfg cfg = get_chart_cfg (logarithmic, 1, 1, 1, 1 * 365);

    // Generates image
    code = generate_img (cfg);
    if (code != 0)
        goto finalise;

    // Processes image
    code = process_img (cfg);
    if (code != 0)
        goto finalise;

finalise:

    // Frees Bitcoin data
    free_btc_data ();
    return code;
}

