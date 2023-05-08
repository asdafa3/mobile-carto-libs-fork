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

    cglib::bbox2<double> Converter::bbox3tobbox2(const cglib::bbox3<double>& bbox) {
        cglib::vec2<double> min(bbox.min(0), bbox.min(1));
        cglib::vec2<double> max(bbox.max(0), bbox.max(1));
        return cglib::bbox2<double>(min, max);
    }
}