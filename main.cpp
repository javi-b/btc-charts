/**
 * @file main.cpp
 * Main file containing project's main function.
 */
#include "utils.h"
#include "btc_chart.h"

/**
 * Main function.
 */
int main() {

    BtcChart::Cfg cfg = BtcChart::Cfg(1024, 576, 0, 5000,
            0.1f * 0.9f, 10000000.0f * 1.1f, utils::kLogarithmic);

    BtcChart btc_chart;
    btc_chart.Generate("charts/chart.png", cfg);

    return 0;
}
