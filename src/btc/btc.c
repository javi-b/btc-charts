/**
 * Library with my general purpose Bitcoin functions.
 *
 * Javi Bonafonte
 */

#include "btc.h"

#define GEN_YEAR 2009
#define GEN_MONTH 1
#define GEN_DAY 9

#define INITIAL_REWARD 50
#define BLOCKS_PER_HALVING 210000

/**
 * Returns float value of the year of the date of 'days_since_gen'.
 *
 * For example, if 'days_since_gen' was 0, the date would be 2009-01-09 and
 * it would return '2009.0247'.
 */
float days_since_gen_to_years (int days_since_gen) {

    float years = days_since_gen / 365.0 + 9.0 / 365.0 + GEN_YEAR;

    return years;
}

/**
 * Returns Bitcoin block reward at the time of 'days_since_gen'.
 * (Aproximated theorical calculation).
 */
float get_btc_block_reward (int days_since_gen) {

    float reward = INITIAL_REWARD;
    int halvings = (int) (days_since_gen / 1460); // 1460 days in 4 years

    for (int i = 0; i < halvings; i++)
        reward /= 2;

    return reward;
}

/**
 * Returns total stock of Bitcoin at the time of 'days_since_gen'.
 * (Aproximated theorical calculation).
 */
float get_btc_stock (int days_since_gen) {

    float reward = INITIAL_REWARD;
    int halvings = (int) (days_since_gen / 1460); // 1460 days in 4 years
    float stock;

    for (int i = 0; i < halvings; i++) {
        stock += BLOCKS_PER_HALVING * reward;
        reward /= 2;
    }

    int days_since_last_halving = days_since_gen % 1460;
    // One block per every 10 minutes of the day (144 blocks per day)
    int blocks_since_last_halving = days_since_last_halving * 144;

    stock += blocks_since_last_halving * reward;

    return stock;
}

