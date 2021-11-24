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

    BtcChart btc_chart;
    btc_chart.Generate("charts/chart.png", 800, 600, utils::kLogarithmic);
    return 0;
}
