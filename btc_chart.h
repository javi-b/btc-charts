#pragma once

#include "utils.h"
#include "img.h"
#include "btc_data.h"

/**
 * Bitcoin chart class.
 */
class BtcChart {
public:
    // Bitcoin chart config data structure.
    struct Cfg {
        int width, height; // Chart image size.
        // First and last days represented in the chart.
        int day_a, day_b;
        // Bottom and top prices represented in the chart.
        float bottom_price, top_price;
        utils::Scale scale; // Scale of y value in chart.

        // constructor
        Cfg(const int width, const int height,
                const int day_a, const int day_b,
                const float bottom_price, const float top_price,
                const utils::Scale scale);
    };

    // public functions
    void Generate(const std::string & path, Cfg & cfg);

private:
    BtcData btc_data_; ///< BtcData object.

    Cfg * cfg_; ///< Pointer to current chart config.

    // private functions
    void DrawPrice(Img & img);

    // utility private functions
    int XToDay(const int x);
    float PriceToY(const float price);
};
