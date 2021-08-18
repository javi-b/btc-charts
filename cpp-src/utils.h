/**
 * General purpose utilities that can be used anywhere in the whole
 * project.
 */


#pragma once

#include <math.h>


namespace utils {

enum Scale {kLinear, kLogarithmic};

struct RGB {
    int r, g, b;
};

/**
 * Applies 'scale' to 'value' and returns it.
 */
float ApplyScale(const Scale scale, const float value) {

    switch (scale) {

        case kLinear:
            return value;

        case kLogarithmic:
            if (value <= 0)
                return value;
            return float(log(value));

        default:
            return -1;
    }
}

/**
 * Converts HSL color to RGB
 * where 0 <= 'h' < 360, 0 <= 's' < 1, 0 <= 'l' < 1
 */
RGB HslToRgb(const int h, const float s, const float l) {

    RGB rgb;
    float c, x, m, r0 = 0, g0 = 0, b0 = 0;

    c = (1 - fabs (2 * l - 1)) * s;
    x = c * (1 - fabs (fmodf (h / 60.0, 2) - 1));
    m = l - c / 2;

    if ((h >= 0 && h < 60) || (h >= 300 && h < 360))
        r0 = c;
    else if ((h >= 60 && h < 120) || (h >= 240 && h < 300))
        r0 = x;

    if ((h >= 0 && h < 60) || (h >= 180 && h < 240))
        g0 = x;
    else if (h>= 60 && h < 180)
        g0 = c;

    if ((h >= 120 && h < 180) || (h >= 300 && h < 360))
        b0 = x;
    else if (h>= 180 && h < 300)
        b0 = c;

    rgb.r = round ((r0 + m) * 255);
    rgb.g = round ((g0 + m) * 255);
    rgb.b = round ((b0 + m) * 255);

    return rgb;
}

}
