/**
 * My Bitcoin data library. Gets data from a CSV file from
 * 'bitcoinity.org'.
 *
 * When using it always start by calling 'read_btc_data ()' and, when all
 * the needed data has been obtained, finish by calling 'free_btc_data ()'.
 *
 * Javi Bonafonte
 */

#include <stdlib.h>
#include <stdio.h>

#include "btcdata.h"

#define STR_LEN 128 // Default length of strings

#define BTC_CSV_PATH "data/bitcoinity_data.csv"
#define DAYS_FROM_GEN 554 // Days from genesis block to first day in file
                          // Genesis block was on 2009-01-09

struct date {
    int y, m, d;
};

// Struct with BTC price for a date (represents one row of the CSV file)
struct row {
    int days_since_gen;
    struct date date;
    float price;
};

static int Num_Days;
static struct row *Rows;

/**
 * Returns the number of days from the genesis block to the first day in
 * the Bitcoin CSV file.
 *
 * Genesis block was on 2009-01-09.
 */
int get_num_days_from_gen_to_first_btc_data () {

    return DAYS_FROM_GEN;
}

/**
 * Returns number of lines in file pointed by 'fp'.
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
 * Returns average price of all the available exchanges prices in one row
 * of the Bitcoin CSV file or -1 if no price is found.
 *
 * (Specific to data from 'bitcoinity.org').
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
 * Converts date string with format 'year-month-day' to a date struct.
 */
struct date string_to_date (char *date_string) {

    struct date date;
    sscanf (date_string, "%d-%d-%d", &date.y, &date.m, &date.d);
    return date;
}

/**
 * Saves all rows except the titles one from file pointed by 'fp' in the 
 * 'Rows' array.
 */
void set_rows (FILE *fp, int num_rows) {

    // Allocates memory for the array of rows
    Rows = (struct row *) malloc (num_rows * sizeof (struct row));

    rewind (fp);

    fscanf (fp, "%*[^\n]\n"); // Reads first line (titles line)

    // Reads rest of lines and saves them in 'Rows'
    // (Specific for 'bitcoinity_data.csv')

    char date_string[STR_LEN];

    for (int i = 0; fscanf (fp, "%[^,]", date_string) != EOF; i++) {

        Rows[i].days_since_gen = i + DAYS_FROM_GEN;
        Rows[i].date = string_to_date (date_string);
        Rows[i].price = get_row_avg_price (fp);
    }
}

/**
 * Reads Bitcoin data from file 'BTC_CSV_PATH' and saves it as global
 * variables.
 *
 * Must be the first function to run of this library!!!
 */
int read_btc_data () {

    // Opens Bitcoin CSV file for reading
    FILE *fp = fopen (BTC_CSV_PATH, "r");
    if (fp == NULL) {
        fprintf (stderr, "Couldn't open '%s' file\n", BTC_CSV_PATH);
        return 1;
    }

    // Gets data from Bitcoin CSV file
    Num_Days = get_num_lines (fp) - 1;
    set_rows (fp, Num_Days);

    // Closes Bitcoin CSV file
    fclose (fp);

    return 0;
}

/**
 * Returns the number of days in the Bitcoin CSV file
 * or 0 if no data was found.
 */
int get_num_btc_data_days () {

    // If 'Rows' array hasn't been created...
    if (Rows == NULL) {
        fprintf (stderr, "Rows array hasn't been created.\n");
        return 0;
    }

    return Num_Days;
}

/**
 * Returns average price between 'row_a' and 'row_b' (including 'row_b')
 * or -1 if no data was found.
 */
float get_avg_price (int row_a, int row_b) {

    // If 'Rows' array hasn't been created...
    if (Rows == NULL) {
        fprintf (stderr, "Rows array hasn't been created.\n");
        return -1;
    }

    float price, avg_price = 0;
    int null_prices = 0, num_prices;

    for (int row = row_a + 1; row <= row_b; row++) {

        price = Rows[row].price;

        if (price > 0) // If the row has a price...
            avg_price += Rows[row].price;
        else
            null_prices++;

    }

    num_prices = row_b - row_a - null_prices;

    if (avg_price <= 0 || num_prices <= 0)
        return -1;

    return avg_price / (row_b - row_a - null_prices);
}

/**
 * Frees 'Rows' array.
 *
 * Must be the last function to run of this library!!!
 */
void free_btc_data () {

    if (Rows != NULL)
        free (Rows);
}

