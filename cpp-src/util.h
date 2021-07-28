#pragma once


namespace util {

    enum Scale {kLinear, kLogarithmic};

    struct RGB {
        int r, g, b;
    };

    float ApplyScale(Scale scale, float value);
    RGB HslToRgb(int h, float s, float l);

}
