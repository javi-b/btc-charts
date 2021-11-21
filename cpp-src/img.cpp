#include <iostream>
#include <fstream>
#include "img.h"


/**
 * Constructor.
 * Creates the image and sets its background.
 *
 * @param width,height Image width ang height sizes.
 * @param pad Padding for image.
 * @param bg_color Background color for image.
 */
Img::Img(const int width, const int height, const int pad,
            const std::string & bg_color)
        : width_(width), height_(height), pad_(pad) {

    img_ = Magick::Image(Magick::Geometry(width_, height_),
            Magick::Color(bg_color));
}

/**
 * Writes image in path.
 *
 * @param path Path where image should be written.
 */
void Img::Write(const std::string & path) {

    std::ofstream file(path, std::ios::trunc); // creates or truncates file
    if (file.is_open()) {
        file.close(); // closes file
        img_.write(path); // writes img in file
    } else {
        std::cerr << "Error: could not open " << path << '\n';
    }
    
}
