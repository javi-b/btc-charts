#pragma once

#include <string>
#include <vector>
#include "utils.h"

/**
 * Bitcoin data class.
 */
class BtcData {

public:
    // constructor and destructor
    BtcData();

private:
    /// Bitcoin data from one day data structure.
    struct DayBtcData {
        int days_since_gen = 0;
        struct utils::Date date;
        float price = 0.0f;
    };

    static const std::string kDataFilePath_;
    static const int kInitialDaysSinceGen;

    /// Vector of all days Bitcoin data.
    std::vector<DayBtcData> btc_data_;

    // helper functions
    DayBtcData ExtractDayBtcData(const std::string & line);
    utils::Date ExtractDate(const std::string & date_str);
    float ExtractAveragePrice(const std::string & prices_str);
};
