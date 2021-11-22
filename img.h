#pragma once

#include <Magick++.h>
#include "utils.h"


/**
 * Image class.
 */
class Img {
public:
    // constructor
    Img(const int width, const int height, const std::string & bg_color);

    // public functions

    void DrawLine(const float start_x, const float start_y,
            const float end_x, const float end_y,
            const std::string & color);

    void Write(const std::string & path);

private:
    Magick::Image img_ = NULL; ///< Magick++ Image object.
};
