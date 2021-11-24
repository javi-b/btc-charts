#pragma once

#include <string>
#include <vector>
#include "utils.h"

/**
 * Bitcoin data class.
 */
class BtcData {
public:
    /// Bitcoin data from one day data structure.
    struct DayBtcData {
        int days_since_gen = 0;
        struct utils::Date date;
        float price = 0.0f;
    };

    // constructor
    BtcData();

    // public functions
    float GetPrice(const int day_since_gen);
    float GetAvgPrice(const int day_a, const int day_b);
    float GetMaxPrice(const int day_a, const int day_b);
    int GetLastDay();

private:
    static const std::string kDataFilePath_;
    static const int kInitialDaysSinceGen_;

    /// Vector of all days Bitcoin data.
    std::vector<DayBtcData> btc_data_;

    // helper functions
    DayBtcData ExtractDayBtcData(const std::string & line);
    static utils::Date ExtractDate(const std::string & date_str);
    static float ExtractAveragePrice(const std::string & prices_str);
    float FindMaxPrice();
};
