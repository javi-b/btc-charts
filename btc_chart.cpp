#include <iostream>
#include "btc_chart.h"

/**
 * Bitcoin chart config data structure constructor.
 */
BtcChart::Cfg::Cfg(const int width, const int height,
        const int day_a, const int day_b,
        const float bottom_price, const float top_price,
        const utils::Scale scale)
    : width(width), height(height), day_a(day_a), day_b(day_b),
        bottom_price(bottom_price), top_price(top_price), scale(scale) {}

/**
 * Generates a Bitcoin chart image.
 *
 * @param path Path where to save chart image.
 * @param cfg Bitcoin chart config data structure.
 */
void BtcChart::Generate(const std::string & path, Cfg & cfg) {

    cfg_ = &cfg;

    Img img(cfg_->width, cfg_->height);
    DrawPrice(img);

    img.Write(path);
}

/**
 * Draws Bitcoin price on chart image.
 *
 * @param img Image object.
 */
void BtcChart::DrawPrice(Img & img) {

    float prev_y = -1.0f;

    for (int x = 0; x < cfg_->width; x++) {

        const int start_day = XToDay(x - 1);
        const int end_day = XToDay(x);

        const float price = btc_data_.GetAvgPrice(start_day, end_day);
        const float y = (price == -1.0f) ? price : PriceToY(price);

        if (prev_y != -1.0f && y != -1.0f)
            img.DrawLine(float(x - 1), prev_y, float(x), y, "#000000");

        prev_y = y;
    }
}

/**
 * Gets day according to x position in chart image.
 *
 * @param x X position.
 * @return Day according to x position.
 */
int BtcChart::XToDay(const int x) {

    return (cfg_->day_a + x * (cfg_->day_b - cfg_->day_a) / cfg_->width);
}

/**
 * Gets y position in chart image according to price.
 *
 * @param price Price to convert.
 * @return Y position.
 */
float BtcChart::PriceToY(const float price) {

    const float scaled_price = utils::ApplyScale(cfg_->scale, price);
    const float scaled_bottom_price = utils::ApplyScale(cfg_->scale,
            cfg_->bottom_price);
    const float scaled_top_price = utils::ApplyScale(cfg_->scale,
            cfg_->top_price);

    return (cfg_->height - (scaled_price - scaled_bottom_price)
            * cfg_->height / scaled_top_price);
}
