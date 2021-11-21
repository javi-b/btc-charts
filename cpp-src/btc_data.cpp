#include <iostream>
#include <fstream>
#include <sstream>
#include "btc_data.h"


/// Bitcoin data CSV file path.
const std::string BtcData::kDataFilePath_ = "data/bitcoinity_price.csv";

/// Days from genesis block to first day in data file.
/// Genesis block was on 2009-01-09.
const int BtcData::kInitialDaysSinceGen = 554;

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
            day_btc_data.days_since_gen = kInitialDaysSinceGen + i;
            btc_data_.push_back(day_btc_data);
        }

        file.close();

    } else {
        std::cerr << "Error: could not open " << kDataFilePath_ << '\n';
    }
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
 * by commas.
 *
 * @param prices_str String containg multiple prices separated by commas.
 * @return Average price.
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

    return utils::GetAverage(prices);
}
