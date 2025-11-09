#include "assic_fonts.h"
#include "image_painter.h"

#include "slam_log_reporter.h"
#include "slam_memory.h"
#include "slam_operations.h"

namespace image_painter {

template void ImagePainter::DrawSolidRectangle<GrayImage, uint8_t>(GrayImage &image, int32_t x, int32_t y, int32_t width, int32_t height, const uint8_t &color);
template void ImagePainter::DrawSolidRectangle<RgbImage, RgbPixel>(RgbImage &image, int32_t x, int32_t y, int32_t width, int32_t height, const RgbPixel &color);
template <typename ImageType, typename PixelType>
void ImagePainter::DrawSolidRectangle(ImageType &image, int32_t x, int32_t y, int32_t width, int32_t height, const PixelType &color) {
    if (image.data() == nullptr || width < 0 || height < 0) {
        return;
    }
    for (int32_t u = x; u < x + width; ++u) {
        CONTINUE_IF(u > image.cols() - 1);
        for (int32_t v = y; v < y + height; ++v) {
            BREAK_IF(v > image.rows() - 1);
            image.SetPixelValue(v, u, color);
        }
    }
}

template void ImagePainter::DrawHollowRectangle<GrayImage, uint8_t>(GrayImage &image, int32_t x, int32_t y, int32_t width, int32_t height,
                                                                    const uint8_t &color);
template void ImagePainter::DrawHollowRectangle<RgbImage, RgbPixel>(RgbImage &image, int32_t x, int32_t y, int32_t width, int32_t height,
                                                                    const RgbPixel &color);
template <typename ImageType, typename PixelType>
void ImagePainter::DrawHollowRectangle(ImageType &image, int32_t x, int32_t y, int32_t width, int32_t height, const PixelType &color) {
    if (image.data() == nullptr || width < 0 || height < 0) {
        return;
    }

    const int32_t x0 = x;
    const int32_t x1 = x + width;
    const int32_t y0 = y;
    const int32_t y1 = y + height;

    for (int32_t u = x; u < x1; ++u) {
        image.SetPixelValue(y0, u, color);
        image.SetPixelValue(y1, u, color);
    }

    for (int32_t v = y; v < y1; ++v) {
        image.SetPixelValue(v, x0, color);
        image.SetPixelValue(v, x1, color);
    }
}

template void ImagePainter::DrawBressenhanLine<GrayImage, uint8_t>(GrayImage &image, int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint8_t &color);
template void ImagePainter::DrawBressenhanLine<RgbImage, RgbPixel>(RgbImage &image, int32_t x1, int32_t y1, int32_t x2, int32_t y2, const RgbPixel &color);
template <typename ImageType, typename PixelType>
void ImagePainter::DrawBressenhanLine(ImageType &image, int32_t x1, int32_t y1, int32_t x2, int32_t y2, const PixelType &color) {
    if (image.data() == nullptr) {
        return;
    }

    int32_t dx = std::abs(x2 - x1);
    int32_t dy = std::abs(y2 - y1);
    bool is_larger_than_45_deg = false;
    if (dx < dy) {
        is_larger_than_45_deg = true;
        SlamOperation::ExchangeValue(x1, y1);
        SlamOperation::ExchangeValue(x2, y2);
        SlamOperation::ExchangeValue(dx, dy);
    }

    // Precompute some temp variables.
    const int32_t ix = (x2 - x1) > 0 ? 1 : -1;
    const int32_t iy = (y2 - y1) > 0 ? 1 : -1;
    int32_t cx = x1;
    int32_t cy = y1;
    const int32_t dy_2 = dy * 2;
    const int32_t dy_dx_2 = (dy - dx) * 2;
    int32_t dy_2_dx = dy * 2 - dx;

    // Draw line.
    while (cx != x2) {
        if (dy_2_dx < 0) {
            dy_2_dx += dy_2;
        } else {
            cy += iy;
            dy_2_dx += dy_dx_2;
        }

        if (is_larger_than_45_deg) {
            image.SetPixelValue(cx, cy, color);
        } else {
            image.SetPixelValue(cy, cx, color);
        }
        cx += ix;
    }
}

template void ImagePainter::DrawNaiveLine<GrayImage, uint8_t>(GrayImage &image, int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint8_t &color);
template void ImagePainter::DrawNaiveLine<RgbImage, RgbPixel>(RgbImage &image, int32_t x1, int32_t y1, int32_t x2, int32_t y2, const RgbPixel &color);
template <typename ImageType, typename PixelType>
void ImagePainter::DrawNaiveLine(ImageType &image, int32_t x1, int32_t y1, int32_t x2, int32_t y2, const PixelType &color) {
    bool is_steep = false;

    if (std::abs(x1 - x2) < std::abs(y1 - y2)) {
        SlamOperation::ExchangeValue(x1, y1);
        SlamOperation::ExchangeValue(x2, y2);
        is_steep = true;
    }

    if (x1 > x2) {
        SlamOperation::ExchangeValue(x1, x2);
        SlamOperation::ExchangeValue(y1, y2);
    }

    for (int32_t x = x1; x <= x2; ++x) {
        const float lambda = static_cast<float>(x - x1) / static_cast<float>(x2 - x1);
        int32_t y = static_cast<int32_t>(static_cast<float>(y1) * (1.0f - lambda) + static_cast<float>(y2) * lambda);
        if (is_steep) {
            image.SetPixelValue(x, y, color);
        } else {
            image.SetPixelValue(y, x, color);
        }
    }
}

template void ImagePainter::DrawDashedLine<GrayImage, uint8_t>(GrayImage &image, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t step,
                                                               const uint8_t &color);
template void ImagePainter::DrawDashedLine<RgbImage, RgbPixel>(RgbImage &image, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t step,
                                                               const RgbPixel &color);
template <typename ImageType, typename PixelType>
void ImagePainter::DrawDashedLine(ImageType &image, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t step, const PixelType &color) {
    bool is_steep = false;

    if (std::abs(x1 - x2) < std::abs(y1 - y2)) {
        SlamOperation::ExchangeValue(x1, y1);
        SlamOperation::ExchangeValue(x2, y2);
        is_steep = true;
    }

    if (x1 > x2) {
        SlamOperation::ExchangeValue(x1, x2);
        SlamOperation::ExchangeValue(y1, y2);
    }

    for (int32_t x = x1; x <= x2; x += step) {
        const float lambda = static_cast<float>(x - x1) / static_cast<float>(x2 - x1);
        const int32_t y = static_cast<int32_t>(static_cast<float>(y1) * (1.0f - lambda) + static_cast<float>(y2) * lambda);
        if (is_steep) {
            image.SetPixelValue(x, y, color);
        } else {
            image.SetPixelValue(y, x, color);
        }
    }

    // Draw the end point.
    const float lambda = static_cast<float>(x2 - x1) / static_cast<float>(x2 - x1);
    const int32_t y = static_cast<int32_t>(static_cast<float>(y1) * (1.0f - lambda) + static_cast<float>(y2) * lambda);
    if (is_steep) {
        image.SetPixelValue(x2, y, color);
    } else {
        image.SetPixelValue(y, x2, color);
    }
}

template void ImagePainter::DrawSolidCircle<GrayImage, uint8_t>(GrayImage &image, int32_t center_x, int32_t center_y, int32_t radius, const uint8_t &color);
template void ImagePainter::DrawSolidCircle<RgbImage, RgbPixel>(RgbImage &image, int32_t center_x, int32_t center_y, int32_t radius, const RgbPixel &color);
template <typename ImageType, typename PixelType>
void ImagePainter::DrawSolidCircle(ImageType &image, int32_t center_x, int32_t center_y, int32_t radius, const PixelType &color) {
    const int32_t x0 = center_x - radius;
    const int32_t y0 = center_y - radius;
    const int32_t x1 = center_x + radius;
    const int32_t y1 = center_y + radius;
    const float radius_f = static_cast<float>(radius);

    for (int32_t y = y0; y <= y1; ++y) {
        for (int32_t x = x0; x <= x1; ++x) {
            if (std::hypot(y - center_y, x - center_x) < radius_f) {
                image.SetPixelValue(y, x, color);
            }
        }
    }
}

template void ImagePainter::DrawHollowCircle<GrayImage, uint8_t>(GrayImage &image, int32_t center_x, int32_t center_y, int32_t radius, const uint8_t &color);
template void ImagePainter::DrawHollowCircle<RgbImage, RgbPixel>(RgbImage &image, int32_t center_x, int32_t center_y, int32_t radius, const RgbPixel &color);
template <typename ImageType, typename PixelType>
void ImagePainter::DrawHollowCircle(ImageType &image, int32_t center_x, int32_t center_y, int32_t radius, const PixelType &color) {
    const int32_t x0 = center_x - radius;
    const int32_t y0 = center_y - radius;
    const int32_t x1 = center_x + radius;
    const int32_t y1 = center_y + radius;
    const float radius_out = static_cast<float>(radius);
    const float radius_in = static_cast<float>(radius) - 1.1f;

    for (int32_t y = y0; y <= y1; ++y) {
        for (int32_t x = x0; x <= x1; ++x) {
            const float distance = std::hypot(y - center_y, x - center_x);
            if (distance < radius_out && distance > radius_in) {
                image.SetPixelValue(y, x, color);
            }
        }
    }
}

template void ImagePainter::DrawMidBresenhamEllipse<GrayImage, uint8_t>(GrayImage &image, int32_t center_x, int32_t center_y, int32_t radius_x,
                                                                        int32_t radius_y, const uint8_t &color);
template void ImagePainter::DrawMidBresenhamEllipse<RgbImage, RgbPixel>(RgbImage &image, int32_t center_x, int32_t center_y, int32_t radius_x, int32_t radius_y,
                                                                        const RgbPixel &color);
template <typename ImageType, typename PixelType>
void ImagePainter::DrawMidBresenhamEllipse(ImageType &image, int32_t center_x, int32_t center_y, int32_t radius_x, int32_t radius_y, const PixelType &color) {
    int32_t y = 0;
    int32_t x = radius_x;
    const float a = radius_y;
    const float b = radius_x;
    const float a2 = a * a;
    const float b2 = b * b;
    image.SetPixelValue(center_y + y, center_x + x, color);
    image.SetPixelValue(center_y - y, center_x - x, color);
    image.SetPixelValue(center_y - y, center_x + x, color);
    image.SetPixelValue(center_y + y, center_x - x, color);

    float d1 = b2 + a2 * (0.5f - b);
    while (b2 * (y + 1) < a2 * (x - 0.5f)) {
        if (d1 <= 0) {
            d1 += b2 * (2 * y + 3);
            ++y;
        } else {
            d1 += b2 * (2 * y + 3) + a2 * (2 - 2 * x);
            ++y;
            --x;
        }
        image.SetPixelValue(center_y + y, center_x + x, color);
        image.SetPixelValue(center_y - y, center_x - x, color);
        image.SetPixelValue(center_y - y, center_x + x, color);
        image.SetPixelValue(center_y + y, center_x - x, color);
    }

    float d2 = b2 * (y + 0.5f) * (y + 0.5f) + a2 * (x - 1) * (x - 1) - a2 * b2;
    while (x > 0) {
        if (d2 <= 0) {
            d2 += b2 * (2 * y + 2) + a2 * (3 - 2 * x);
            ++y;
            --x;
        } else {
            d2 += a2 * (3 - 2 * x);
            --x;
        }
        image.SetPixelValue(center_y + y, center_x + x, color);
        image.SetPixelValue(center_y - y, center_x - x, color);
        image.SetPixelValue(center_y - y, center_x + x, color);
        image.SetPixelValue(center_y + y, center_x - x, color);
    }
}

template void ImagePainter::DrawTrustRegionOfGaussian<GrayImage, uint8_t>(GrayImage &image, const Vec2 &center, const Mat2 &covariance, const uint8_t &color,
                                                                          const float sigma_scale);
template void ImagePainter::DrawTrustRegionOfGaussian<RgbImage, RgbPixel>(RgbImage &image, const Vec2 &center, const Mat2 &covariance, const RgbPixel &color,
                                                                          const float sigma_scale);
template <typename ImageType, typename PixelType>
void ImagePainter::DrawTrustRegionOfGaussian(ImageType &image, const Vec2 &center, const Mat2 &covariance, const PixelType &color, const float sigma_scale) {
    // Decompose covariance matrix.
    const Eigen::SelfAdjointEigenSolver<Mat2> saes(covariance);
    const Vec2 &eigen_values = saes.eigenvalues();
    const Mat2 &eigen_vectors = saes.eigenvectors();
    const float cos_theta = eigen_vectors(0, 0);
    const float sin_theta = eigen_vectors(1, 0);
    const float a = std::sqrt(eigen_values(1)) * 0.5f * sigma_scale;
    const float b = std::sqrt(eigen_values(0)) * 0.5f * sigma_scale;

    // Compute step length with long_radius.
    const float step = 1.0f / std::max(a, b);
    RETURN_IF(std::isinf(step) || std::isnan(step));

    // Draw rotated ellipse.
    for (float angle = 0.0f; angle < 6.28f; angle += step) {
        const float cos_angle = std::cos(angle);
        const float sin_angle = std::sin(angle);
        const float x = center.x() + b * cos_angle * cos_theta - a * sin_angle * sin_theta;
        const float y = center.y() + b * cos_angle * sin_theta + a * sin_angle * cos_theta;

        const int32_t int_x = static_cast<int32_t>(x);
        const int32_t int_y = static_cast<int32_t>(y);
        image.SetPixelValue(y - static_cast<float>(int_y) > 0.5f ? int_y + 1 : int_y, x - static_cast<float>(int_x) > 0.5f ? int_x + 1 : int_x, color);
    }
}

template void ImagePainter::DrawCharacter<GrayImage, uint8_t>(GrayImage &image, char character, int32_t x, int32_t y, const uint8_t &color, int32_t font_size);
template void ImagePainter::DrawCharacter<RgbImage, RgbPixel>(RgbImage &image, char character, int32_t x, int32_t y, const RgbPixel &color, int32_t font_size);
template <typename ImageType, typename PixelType>
void ImagePainter::DrawCharacter(ImageType &image, char character, int32_t x, int32_t y, const PixelType &color, int32_t font_size) {
    const int32_t idx = static_cast<int32_t>(character - ' ');
    const int32_t size = ((font_size >> 3) + ((font_size % 8) ? 1 : 0)) * (font_size >> 1);
    int32_t y0 = y;

    for (int32_t i = 0; i < size; ++i) {
        // Get the item in fonts with index.
        uint8_t item = 0;
        switch (font_size) {
            default:
            case 12: {
                item = AssicFonts::ascii_1206()[idx][i];
                break;
            }
            case 16: {
                item = AssicFonts::ascii_1608()[idx][i];
                break;
            }
            case 24: {
                item = AssicFonts::ascii_2412()[idx][i];
                break;
            }
        }

        // Draw pixel based on this item.
        for (int32_t j = 0; j < 8; ++j) {
            if (item & 0x80) {
                image.SetPixelValue(y, x, color);
            }

            item <<= 1;
            ++y;
            if (y - y0 == font_size) {
                y = y0;
                ++x;
                break;
            }
        }
    }
}

template void ImagePainter::DrawString<GrayImage, uint8_t>(GrayImage &image, const std::string &str, int32_t x, int32_t y, const uint8_t &color,
                                                           int32_t font_size);
template void ImagePainter::DrawString<RgbImage, RgbPixel>(RgbImage &image, const std::string &str, int32_t x, int32_t y, const RgbPixel &color,
                                                           int32_t font_size);
template <typename ImageType, typename PixelType>
void ImagePainter::DrawString(ImageType &image, const std::string &str, int32_t x, int32_t y, const PixelType &color, int32_t font_size) {
    if (font_size != 12 && font_size != 16 && font_size != 24) {
        font_size = 12;
    }
    const int32_t dx = font_size >> 1;
    for (const auto &chr: str) {
        DrawCharacter(image, chr, x, y, color, font_size);
        x += dx;
    }
}

}  // namespace image_painter
