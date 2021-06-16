/**
 * Library with my general purpose functions.
 *
 * Javi Bonafonte
 */

#include <math.h>

#include "util.h"

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

