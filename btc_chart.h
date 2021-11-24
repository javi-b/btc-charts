#pragma once

#include "utils.h"
#include "img.h"
#include "btc_data.h"

/**
 * BtcChart class.
 */
class BtcChart {
public:
    // public functions
    void Generate(const std::string & path, const int width,
            const int height);

private:
    BtcData btc_data_; ///< BtcData object.

    int width_ = 0; ///< Chart width.
    int height_ = 0; ///< Chart height.

    // private functions
    void DrawPrice(Img & img, const int day_a, const int day_b);

    // utility private functions
    int XToDay(const int day_a, const int day_b, const int x);
    float PriceToY(const float min_price, const float max_price,
            const float price);
};
