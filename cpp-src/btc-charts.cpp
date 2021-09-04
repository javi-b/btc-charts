/**
 * @file btc-charts.cpp
 * Main file containing project's main function.
 */
#include <iostream>
#include "utils.h"
#include "img.h"

// image constants
#define IMG_PATH "chart.png" ///< Output image path.
#define WIDTH 1024 ///< Output image width.
#define HEIGHT 576 ///< Output image height.

/**
 * Main function.
 */
int main() {
    
    Img img = Img(WIDTH, HEIGHT, 18, "#ffffff");
    img.Write(IMG_PATH);

    std::cout << "hi" << std::endl;

    return 0;
}
