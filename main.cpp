/**
 * @file main.cpp
 * Main file containing project's main function.
 */
#include "btc_chart.h"

/**
 * Main function.
 */
int main() {

    BtcChart btc_chart;
    btc_chart.Generate("charts/chart.png", 1600, 900);
    return 0;
}
