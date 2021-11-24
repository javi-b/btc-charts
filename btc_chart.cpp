#include <iostream>
#include "btc_chart.h"

/**
 * Generates a Bitcoin chart image.
 *
 * @param path Path where to save chart image.
 * @param width,height Chart image size.
 * @param scale Scale of y values in chart.
 */
void BtcChart::Generate(const std::string & path, const int width,
        const int height, const utils::Scale scale) {

    width_ = width;
    height_ = height;

    Img img(width_, height_);
    DrawPrice(img, scale, 0, btc_data_.GetLastDay());

    img.Write(path);
}

/**
 * Draws Bitcoin price on chart image.
 *
 * @param img Image object.
 * @param scale Scale of y values in chart.
 * @param day_a,day_b First and last days to draw the Bitcoin price of.
 */
void BtcChart::DrawPrice(Img & img, const utils::Scale scale,
        const int day_a, const int day_b) {

    // price represented at the topmost point of the chart
    const float top_price = btc_data_.GetMaxPrice(day_a, day_b);

    float prev_y;

    for (int x = 1; x < width_; x++) {

        const int start_day = XToDay(day_a, day_b, x - 1);
        const int end_day = XToDay(day_a, day_b, x);

        const float y = PriceToY(scale, 0, top_price,
                btc_data_.GetAvgPrice(start_day, end_day));

        img.DrawLine(float(x - 1), (x == 1) ? y : prev_y,
                float(x), y, "#000000");

        prev_y = y;
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
 * @param scale Scale of y values in chart.
 * @param min_price,max_price Minimum and maximum prices to show on the
 * chart image.
 * @param price Price to convert.
 * @return Y position.
 */
float BtcChart::PriceToY(const utils::Scale scale, const float min_price,
        const float max_price, const float price) {

    return (height_ - (utils::ApplyScale(scale, price)
                - utils::ApplyScale(scale, min_price))
            * height_ / utils::ApplyScale(scale, max_price));
}
