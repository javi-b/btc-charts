#include <iostream>
#include <fstream>
#include <sstream>
#include "btc_data.h"


/// Bitcoin data CSV file path.
const std::string BtcData::kDataFilePath_ = "data/bitcoinity_price.csv";

/// Days from genesis block to first day in data file.
/// Genesis block was on 2009-01-09.
const int BtcData::kInitialDaysSinceGen_ = 554;

/**
 * Constructor.
 * Extracts data from file.
 */
BtcData::BtcData() {

    std::ifstream file(kDataFilePath_);

    if (file.is_open()) {

        std::string line;

        getline(file, line); // gets first line (titles line)

        // extracts data from the rest of lines
        for (int i = 0; getline(file, line); i++) {
            auto day_btc_data = ExtractDayBtcData(line);
            day_btc_data.days_since_gen = kInitialDaysSinceGen_ + i;
            btc_data_.push_back(day_btc_data);
        }

        file.close();

    } else {
        std::cerr << "Error: could not open " << kDataFilePath_ << '\n';
    }
}

/**
 * Gets Bitcoin price on a specific day or -1 if there is no data to get
 * the price from
 *
 * @param days_since_gen Days since genesis block to day which price has
 * to be returned.
 * @return Price or -1.
 */
float BtcData::GetPrice(const int days_since_gen) {

    return ((days_since_gen - kInitialDaysSinceGen_ >= int(btc_data_.size())
                || days_since_gen < kInitialDaysSinceGen_)
            ? -1.0f
            : btc_data_[days_since_gen - kInitialDaysSinceGen_].price);
}

/**
 * Get average price between two days or -1 if there is no data to get the
 * price from.
 *
 * @param day_a First day, counted as number of days since genesis block.
 * @param day_b Second day, counted as number of days since genesis block.
 * @return Average price or -1.
 */
float BtcData::GetAvgPrice(const int day_a, const int day_b) {

    int num_days = 0;
    float avg_price = 0.0f;

    for (int day = day_a; day < day_b; day++) {

        const int i = day - kInitialDaysSinceGen_;

        if (i < 0 || i >= int(btc_data_.size()) || btc_data_[i].price < 0)
            continue;

        num_days++;
        avg_price += btc_data_[i].price;
    }

    return ((num_days == 0) ? -1.0f : avg_price / num_days);
}

/**
 * Gets maximum price between two days.
 *
 * @param day_a First day, counted as number of days since genesis block.
 * @param day_b Second day, counted as number of days since genesis block.
 * @return Maximum price.
 */
float BtcData::GetMaxPrice(const int day_a, const int day_b) {

    float max_price = 0.0f;

    for (int day = day_a; day < day_b; day++) {
        const int i = day - kInitialDaysSinceGen_;
        if (i < 0 || i >= int(btc_data_.size()) || btc_data_[i].price < 0)
            continue;
        if (btc_data_[i].price > max_price)
            max_price = btc_data_[i].price;
    }

    return max_price;
}

/**
 * Gets last day of data.
 *
 * @return Last day of data.
 */
int BtcData::GetLastDay() {

    return btc_data_.rbegin()->days_since_gen;
}

/**
 * Extracts one day Bitcoin data from one line of the data file.
 *
 * @param line String containg data from one line of the data file.
 * @return One day Bitcoin data.
 */
BtcData::DayBtcData BtcData::ExtractDayBtcData(const std::string & line) {

    DayBtcData day_btc_data;

    std::stringstream line_ss;
    line_ss << line;

    for (int i = 0; !line_ss.eof(); i++) {

        std::string word;
        line_ss >> word; // exctracts until space

        switch (i) {
        case 0:
            day_btc_data.date = ExtractDate(word);
            break;
        case 2:
            day_btc_data.price = ExtractAveragePrice(word);
            break;
        }
    }

    return day_btc_data;
}

/**
 * Extracts date from string.
 *
 * @param date_str String containg date.
 * @return Date data structure.
 */
utils::Date BtcData::ExtractDate(const std::string & date_str) {

    utils::Date date;

    std::stringstream date_ss;
    date_ss << date_str;

    for (int i = 0; !date_ss.eof(); i++) {

        std::string word;
        getline(date_ss, word, '-');

        int number;
        std::stringstream(word) >> number;

        switch (i) {
        case 0:
            date.y = number;
            break;
        case 1:
            date.m = number;
            break;
        case 2:
            date.d = number;
            break;
        }
    }

    return date;
}

/**
 * Extracts average price from string containing multiple prices separated
 * by commas or -1 if there is no data to get the price from.
 *
 * @param prices_str String containg multiple prices separated by commas.
 * @return Average price or -1.
 */
float BtcData::ExtractAveragePrice(const std::string & prices_str) {

    std::stringstream prices_ss;
    prices_ss << prices_str;

    std::vector<float> prices;

    for (int i = 0; !prices_ss.eof(); i++) {

        std::string word;
        getline(prices_ss, word, ',');

        // the first word is not a price
        if (i == 0)
            continue;

        float number;
        if (std::stringstream(word) >> number)
            prices.push_back(number);
    }

    return ((prices.empty()) ? -1.0f : utils::GetAverage(prices));
}
