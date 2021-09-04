#pragma once

#include <Magick++.h>
#include "utils.h"


/**
 * Image class.
 */
class Img {

public:
    // constructor
    Img(const int width, const int height, const int pad,
            const std::string & bg_color);

    // public functions
    void Write(const std::string & path);

private:
    Magick::Image img_ = NULL; ///< Magick++ Image object.
    int width_, height_, pad_;
};
