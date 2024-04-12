#ifndef _IMAGE_PAINTER_H_
#define _IMAGE_PAINTER_H_

#include "datatype_basic.h"
#include "datatype_image.h"

namespace IMAGE_PAINTER {

/* Class Image Painter Declaration. */
class ImagePainter final {

public:
    ImagePainter() = default;
    virtual ~ImagePainter() = default;

    // Support for convertion.
    template <typename Scalar>
    static uint8_t ConvertValueToUint8(Scalar value, Scalar max_value);
    template <typename Scalar>
    static bool ConvertMatrixToImage(const TMat<Scalar> &matrix,
                                     GrayImage &image,
                                     Scalar max_value = 1e3,
                                     int32_t scale = 4);
    template <typename Scalar>
    static bool ConvertMatrixToImage(const TMat<Scalar> &matrix,
                                     RgbImage &image,
                                     Scalar max_value = 1e3,
                                     int32_t scale = 4);
    static void ConvertUint8ToRgb(const uint8_t *gray,
                                  uint8_t *rgb,
                                  int32_t gray_size);
    static void ConvertRgbToUint8(const uint8_t *rgb,
                                  uint8_t *gray,
                                  int32_t gray_size);
    static void ConvertUint8ToRgbAndUpsideDown(const uint8_t *gray,
                                               uint8_t *rgb,
                                               int32_t gray_rows,
                                               int32_t gray_cols);
    static void ConvertRgbToBgrAndUpsideDown(const uint8_t *rgb,
                                             uint8_t *converted_rgb,
                                             int32_t rgb_rows,
                                             int32_t rgb_cols);

    // Support for image draw.
    template <typename ImageType, typename PixelType>
    static void DrawSolidRectangle(ImageType &image, int32_t x, int32_t y, int32_t width, int32_t height, const PixelType &color);
    template <typename ImageType, typename PixelType>
    static void DrawHollowRectangle(ImageType &image, int32_t x, int32_t y, int32_t width, int32_t height, const PixelType &color);
    template <typename ImageType, typename PixelType>
    static void DrawBressenhanLine(ImageType &image, int32_t x1, int32_t y1, int32_t x2, int32_t y2, const PixelType &color);
    template <typename ImageType, typename PixelType>
    static void DrawNaiveLine(ImageType &image, int32_t x1, int32_t y1, int32_t x2, int32_t y2, const PixelType &color);
    template <typename ImageType, typename PixelType>
    static void DrawSolidCircle(ImageType &image, int32_t center_x, int32_t center_y, int32_t radius, const PixelType &color);
    template <typename ImageType, typename PixelType>
    static void DrawHollowCircle(ImageType &image, int32_t center_x, int32_t center_y, int32_t radius, const PixelType &color);
    template <typename ImageType, typename PixelType>
    static void DrawCharacter(ImageType &image, char character, int32_t x, int32_t y, const PixelType &color, int32_t font_size = 12);
    template <typename ImageType, typename PixelType>
    static void DrawString(ImageType &image, const std::string &str, int32_t x, int32_t y, const PixelType &color, int32_t font_size = 12);

};

}

#endif // end of _IMAGE_PAINTER_H_