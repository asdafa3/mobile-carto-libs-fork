//
// Created by Linus Schwarz on 22.05.21.
//

#include "Converter.h"
#include <cmath>

namespace carto {
    double Converter::roundTo(double value, const char &precision) {
        double factor = std::pow(10.0f, (double)precision);
        return std::round(value * factor) / factor;
    }
}