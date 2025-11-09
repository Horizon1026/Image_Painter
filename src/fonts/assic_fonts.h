#ifndef _ASSIC_FONTS_H_
#define _ASSIC_FONTS_H_

#include "array"
#include "basic_type.h"

namespace IMAGE_PAINTER {

// Class AssicFonts Declaration.
class AssicFonts {

public:
    AssicFonts() = default;
    virtual ~AssicFonts() = default;

    static const std::array<std::array<uint8_t, 12>, 95> &ascii_1206() { return ascii_1206_; }
    static const std::array<std::array<uint8_t, 16>, 95> &ascii_1608() { return ascii_1608_; }
    static const std::array<std::array<uint8_t, 36>, 95> &ascii_2412() { return ascii_2412_; }

private:
    static std::array<std::array<uint8_t, 12>, 95> ascii_1206_;
    static std::array<std::array<uint8_t, 16>, 95> ascii_1608_;
    static std::array<std::array<uint8_t, 36>, 95> ascii_2412_;
};

}  // namespace IMAGE_PAINTER

#endif  // end of _ASSIC_FONTS_H_
