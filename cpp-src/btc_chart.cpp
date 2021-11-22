#include <iostream>
#include "btc_chart.h"

/**
 * Generates a Bitcoin chart image.
 *
 * @param path Path where to save chart image.
 * @param width,height Chart image size.
 */
void BtcChart::Generate(const std::string & path, const int width,
        const int height) {

    width_ = width;
    height_ = height;

    Img img(width_, height_, "#ffffff");
    DrawPrice(img, 0, btc_data_.GetLastDay());

    img.Write(path);
}

/**
 * Draws Bitcoin price on chart image.
 *
 * @param img Image object.
 * @param day_a,day_b First and last days to draw the Bitcoin price of.
 */
void BtcChart::DrawPrice(Img & img, const int day_a, const int day_b) {

    for (int x = 1; x < width_; x++) {

        const int start_day = XToDay(day_a, day_b, x - 1);
        const int start_y = PriceToY(0, btc_data_.GetMaxPrice(day_a, day_b),
                btc_data_.GetPrice(start_day));

        const int end_day = XToDay(day_a, day_b, x);
        const int end_y = PriceToY(0, btc_data_.GetMaxPrice(day_a, day_b),
                btc_data_.GetPrice(end_day));

        img.DrawLine(x - 1, start_y, x, end_y, "#000000");
    }
}

/**
 * Gets day according to x position in chart image.
 *
 * @param day_a,day_b First and last days to draw on the chart image.
 * @param x X position.
 * @return Day according to x position.
 */
int BtcChart::XToDay(const int day_a, const int day_b, const int x) {

    return (day_a + x * (day_b - day_a) / width_);
}

/**
 * Gets y position in chart image according to price.
 *
 * @param min_price,max_price Minimum and maximum prices to show on the
 * chart image.
 * @param price Price to convert.
 * @return Y position.
 */
int BtcChart::PriceToY(const float min_price, const float max_price,
            const float price) {

    return int(height_ - (price - min_price) * height_ / max_price);
}