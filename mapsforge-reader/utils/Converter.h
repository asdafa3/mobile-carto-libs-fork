//
// Created by Linus Schwarz on 22.05.21.
//

#ifndef CARTO_MOBILE_SDK_CONVERTER_H
#define CARTO_MOBILE_SDK_CONVERTER_H


namespace carto {
    /**
     * Utility class for general conversation functions.
     */
    class Converter {
    public:
        /**
         * Function to round a double to a specific digit.
         *
         * @param value The value to round.
         * @param precision The digits to round to.
         * @return rounded double value
         */
        static double roundTo(double value, const char &precision);
    };
}


#endif //CARTO_MOBILE_SDK_CONVERTER_H
