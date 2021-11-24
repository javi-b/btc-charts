#include <iostream>
#include <fstream>
#include "img.h"


/**
 * Constructor.
 * Creates the image and sets its background.
 *
 * @param width,height Image width ang height sizes.
 */
Img::Img(const int width, const int height) {

    img_ = Magick::Image(Magick::Geometry(width, height),
            Magick::Color(0, 0, 0, 0));
}

/**
 * Draws line on image.
 *
 * @param start_x,start_y Line start coordinates.
 * @param end_x,end_y Line end coordinates.
 * @param color Line color.
 */
void Img::DrawLine(const float start_x, const float start_y,
        const float end_x, const float end_y, const std::string & color) {

    img_.strokeColor(Magick::Color(color));
    img_.draw(Magick::DrawableLine(start_x, start_y, end_x, end_y));

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
