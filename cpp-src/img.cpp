#include <iostream>
#include <fstream>
#include "img.h"


/**
 * Constructor.
 * Creates the image and sets its background.
 * @param width Width for image.
 * @param height Height for image.
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
 * @param path Path where image should be written.
 */
void Img::Write(const std::string & path) {

    std::ofstream file;
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    try {
        // opens file for output operations
        file.open(path, std::ios::trunc);
        file.close();

        img_.write(path);

    } catch (const std::ofstream::failure & e) {
        std::cerr << "Exception ofstream failure caught\n";
    }
    
}
