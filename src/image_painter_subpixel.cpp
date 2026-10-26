#include "assic_fonts.h"
#include "image_painter.h"
#include "slam_operations.h"

#include "algorithm"
#include "cmath"
#include "initializer_list"

namespace image_painter {
namespace {
    bool Finite(std::initializer_list<float> values) {
        return std::all_of(values.begin(), values.end(), [](float v) {
            return std::isfinite(v);
        });
    }

    uint8_t BlendChannel(uint8_t background, uint8_t foreground, float alpha) {
        return static_cast<uint8_t>(std::lround(background + alpha * (float(foreground) - background)));
    }

    void Blend(GrayImage &image, int x, int y, uint8_t color, float alpha) {
        image.SetPixelValue(y, x, BlendChannel(image.GetPixelValueNoCheck(y, x), color, alpha));
    }

    void Blend(RgbImage &image, int x, int y, RgbPixel color, float alpha) {
        const auto old = image.GetPixelValueNoCheck(y, x);
        image.SetPixelValue(y, x, RgbPixel {BlendChannel(old.r, color.r, alpha), BlendChannel(old.g, color.g, alpha), BlendChannel(old.b, color.b, alpha)});
    }

    // Clip in floating point before converting to integer, including for distant geometry. Each primitive composites at most once per destination pixel.
    template <typename Image, typename Color, typename Coverage>
    void Rasterize(Image &image, float left, float top, float right, float bottom, const Color &color, Coverage coverage) {
        RETURN_IF(!image.data() || image.cols() <= 0 || image.rows() <= 0 || !Finite({left, top, right, bottom}) || left > right || top > bottom || right < 0 ||
                  bottom < 0 || left > image.cols() - 1 || top > image.rows() - 1);
        const int x0 = static_cast<int>(std::floor(std::max(0.0f, left)));
        const int y0 = static_cast<int>(std::floor(std::max(0.0f, top)));
        const int x1 = static_cast<int>(std::ceil(std::min(float(image.cols() - 1), right)));
        const int y1 = static_cast<int>(std::ceil(std::min(float(image.rows() - 1), bottom)));
        for (int y = y0; y <= y1; ++y) {
            for (int x = x0; x <= x1; ++x) {
                const float alpha = std::clamp(coverage(float(x), float(y)), 0.0f, 1.0f);
                if (alpha > 0) {
                    Blend(image, x, y, color, alpha);
                }
            }
        }
    }

    float IntervalCoverage(float pixel, float start, float end) { return std::max(0.0f, std::min(pixel + 0.5f, end) - std::max(pixel - 0.5f, start)); }

    float SegmentDistance(float x, float y, float x1, float y1, float x2, float y2) {
        // Double intermediates avoid overflow for finite, far-offscreen float endpoints.
        const double dx = double(x2) - x1, dy = double(y2) - y1;
        const double length2 = dx * dx + dy * dy;
        const double t = length2 > 0 ? std::clamp(((double(x) - x1) * dx + (double(y) - y1) * dy) / length2, 0.0, 1.0) : 0;
        return static_cast<float>(std::hypot(double(x) - x1 - t * dx, double(y) - y1 - t * dy));
    }

    // First-order signed distance to an ellipse, used only in the one-pixel edge band.
    float EllipseDistance(float x, float y, float a, float b) {
        const double u = double(x) / a, v = double(y) / b;
        const double r = std::hypot(u, v);
        if (r == 0) {
            return -std::min(a, b);
        }
        const double gradient = std::hypot(u / a, v / b) / r;
        return static_cast<float>((r - 1) / gradient);
    }

    bool GlyphBit(unsigned char character, int x, int y, int size) {
        RETURN_FALSE_IF(character < 32 || character > 126 || x < 0 || y < 0 || x >= size / 2 || y >= size);
        const int index = x * ((size + 7) / 8) + y / 8;
        uint8_t bits = size == 24 ? AssicFonts::ascii_2412()[character - 32][index] :
                       size == 16 ? AssicFonts::ascii_1608()[character - 32][index] :
                                    AssicFonts::ascii_1206()[character - 32][index];
        return bits & (0x80 >> (y % 8));
    }
}  // namespace

template <typename ImageType, typename PixelType>
void ImagePainter::DrawSolidRectangle(ImageType &image, float x, float y, float width, float height, const PixelType &color) {
    RETURN_IF(!Finite({x, y, width, height}) || width <= 0 || height <= 0);
    // Like the integer fill, (x,y) is the center of the first pixel, not its corner.
    const float left = x - 0.5f, top = y - 0.5f, right = left + width, bottom = top + height;
    Rasterize(image, left - 0.5f, top - 0.5f, right + 0.5f, bottom + 0.5f, color, [=](float u, float v) {
        return IntervalCoverage(u, left, right) * IntervalCoverage(v, top, bottom);
    });
}

template <typename ImageType, typename PixelType>
void ImagePainter::DrawHollowRectangle(ImageType &image, float x, float y, float width, float height, const PixelType &color) {
    RETURN_IF(!Finite({x, y, width, height}) || width < 0 || height < 0);
    Rasterize(image, x - 1, y - 1, x + width + 1, y + height + 1, color, [=](float u, float v) {
        return 1.0f - std::min({SegmentDistance(u, v, x, y, x + width, y), SegmentDistance(u, v, x, y, x, y + height),
                                SegmentDistance(u, v, x + width, y, x + width, y + height), SegmentDistance(u, v, x, y + height, x + width, y + height)});
    });
}

template <typename ImageType, typename PixelType>
void ImagePainter::DrawBressenhanLine(ImageType &image, float x1, float y1, float x2, float y2, const PixelType &color) {
    RETURN_IF(!Finite({x1, y1, x2, y2}));
    Rasterize(image, std::min(x1, x2) - 1, std::min(y1, y2) - 1, std::max(x1, x2) + 1, std::max(y1, y2) + 1, color, [=](float u, float v) {
        return 1.0f - SegmentDistance(u, v, x1, y1, x2, y2);
    });
}

template <typename ImageType, typename PixelType>
void ImagePainter::DrawNaiveLine(ImageType &image, float x1, float y1, float x2, float y2, const PixelType &color) {
    DrawBressenhanLine(image, x1, y1, x2, y2, color);
}

template <typename ImageType, typename PixelType>
void ImagePainter::DrawSolidCircle(ImageType &image, float center_x, float center_y, float radius, const PixelType &color) {
    RETURN_IF(!Finite({center_x, center_y, radius}) || radius < 0);
    Rasterize(image, center_x - radius - 1, center_y - radius - 1, center_x + radius + 1, center_y + radius + 1, color, [=](float u, float v) {
        return radius + 0.5f - std::hypot(u - center_x, v - center_y);
    });
}

template <typename ImageType, typename PixelType>
void ImagePainter::DrawHollowCircle(ImageType &image, float center_x, float center_y, float radius, const PixelType &color) {
    RETURN_IF(!Finite({center_x, center_y, radius}) || radius < 0);
    Rasterize(image, center_x - radius - 1, center_y - radius - 1, center_x + radius + 1, center_y + radius + 1, color, [=](float u, float v) {
        return 1.0f - std::abs(std::hypot(u - center_x, v - center_y) - radius);
    });
}

template <typename ImageType, typename PixelType>
void ImagePainter::DrawMidBresenhamEllipse(ImageType &image, float center_x, float center_y, float radius_x, float radius_y, const PixelType &color) {
    RETURN_IF(!Finite({center_x, center_y, radius_x, radius_y}) || radius_x < 0 || radius_y < 0);
    if (radius_x == 0 || radius_y == 0) {
        DrawBressenhanLine(image, center_x - radius_x, center_y - radius_y, center_x + radius_x, center_y + radius_y, color);
        return;
    }
    Rasterize(image, center_x - radius_x - 1, center_y - radius_y - 1, center_x + radius_x + 1, center_y + radius_y + 1, color, [=](float u, float v) {
        return 1.0f - std::abs(EllipseDistance(u - center_x, v - center_y, radius_x, radius_y));
    });
}

template <typename ImageType, typename PixelType>
void ImagePainter::DrawDashedLine(ImageType &image, float x1, float y1, float x2, float y2, float step, const PixelType &color) {
    RETURN_IF(!Finite({x1, y1, x2, y2, step}) || step <= 0);
    // Preserve the original dotted-line convention: step is measured on the major axis.
    const double dx = double(x2) - x1, dy = double(y2) - y1;
    const double extent = std::max(std::abs(dx), std::abs(dy));
    const double length2 = dx * dx + dy * dy;
    Rasterize(image, std::min(x1, x2) - 1, std::min(y1, y2) - 1, std::max(x1, x2) + 1, std::max(y1, y2) + 1, color, [=](float u, float v) {
        const double t = length2 > 0 ? std::clamp(((double(u) - x1) * dx + (double(v) - y1) * dy) / length2, 0.0, 1.0) : 0;
        const double index = std::clamp(std::round(t * extent / step), 0.0, std::floor(extent / step));
        const double dot_t = extent > 0 ? index * step / extent : 0;
        const double distance = std::min(std::hypot(double(u) - x1 - dot_t * dx, double(v) - y1 - dot_t * dy), std::hypot(double(u) - x2, double(v) - y2));
        return 1.0f - static_cast<float>(distance);
    });
}

template <typename ImageType, typename PixelType>
void ImagePainter::DrawString(ImageType &image, const std::string &str, float x, float y, const PixelType &color, int32_t font_size) {
    RETURN_IF(!Finite({x, y}) || str.empty());
    if (font_size != 12 && font_size != 16 && font_size != 24) {
        font_size = 12;
    }
    const int width = font_size / 2;
    const float text_width = static_cast<float>(str.size()) * width;
    const auto bit = [&](float u, float v) {
        if (u < 0 || v < 0 || u >= text_width || v >= font_size) {
            return 0.0f;
        }
        const auto glyph = static_cast<size_t>(u / width);
        if (glyph >= str.size()) {
            return 0.0f;
        }
        return GlyphBit(static_cast<unsigned char>(str[glyph]), int(u - glyph * width), int(v), font_size) ? 1.0f : 0.0f;
    };
    Rasterize(image, x - 1, y - 1, x + text_width, y + font_size, color, [&](float u, float v) {
        const float local_x = u - x, local_y = v - y;
        const float ix = std::floor(local_x), iy = std::floor(local_y), fx = local_x - ix, fy = local_y - iy;
        return (1 - fx) * (1 - fy) * bit(ix, iy) + fx * (1 - fy) * bit(ix + 1, iy) + (1 - fx) * fy * bit(ix, iy + 1) + fx * fy * bit(ix + 1, iy + 1);
    });
}

template <typename ImageType, typename PixelType>
void ImagePainter::DrawCharacter(ImageType &image, char character, float x, float y, const PixelType &color, int32_t font_size) {
    DrawString(image, std::string(1, character), x, y, color, font_size);
}

template <typename ImageType, typename PixelType>
void ImagePainter::DrawTrustRegionOfGaussian(ImageType &image, float center_x, float center_y, const Mat2 &covariance, const PixelType &color,
                                             float sigma_scale) {
    RETURN_IF(!Finite({center_x, center_y, sigma_scale}) || sigma_scale <= 0 || !covariance.allFinite() || !covariance.isApprox(covariance.transpose()));
    const Eigen::SelfAdjointEigenSolver<Mat2> solver(covariance);
    RETURN_IF(solver.info() != Eigen::Success || solver.eigenvalues().minCoeff() <= 0);
    // Keep the existing half-sigma radius convention for compatibility.
    const Vec2 radii = solver.eigenvalues().cwiseSqrt() * (0.5f * sigma_scale);
    const Mat2 rotation = solver.eigenvectors();
    const float extent_x = std::hypot(rotation(0, 0) * radii.x(), rotation(0, 1) * radii.y());
    const float extent_y = std::hypot(rotation(1, 0) * radii.x(), rotation(1, 1) * radii.y());
    Rasterize(image, center_x - extent_x - 1, center_y - extent_y - 1, center_x + extent_x + 1, center_y + extent_y + 1, color, [&](float u, float v) {
        const Vec2 local = rotation.transpose() * Vec2(u - center_x, v - center_y);
        return 1.0f - std::abs(EllipseDistance(local.x(), local.y(), radii.x(), radii.y()));
    });
}

template void ImagePainter::DrawSolidRectangle<GrayImage, uint8_t>(GrayImage &, float, float, float, float, const uint8_t &);
template void ImagePainter::DrawHollowRectangle<GrayImage, uint8_t>(GrayImage &, float, float, float, float, const uint8_t &);
template void ImagePainter::DrawBressenhanLine<GrayImage, uint8_t>(GrayImage &, float, float, float, float, const uint8_t &);
template void ImagePainter::DrawNaiveLine<GrayImage, uint8_t>(GrayImage &, float, float, float, float, const uint8_t &);
template void ImagePainter::DrawSolidCircle<GrayImage, uint8_t>(GrayImage &, float, float, float, const uint8_t &);
template void ImagePainter::DrawHollowCircle<GrayImage, uint8_t>(GrayImage &, float, float, float, const uint8_t &);
template void ImagePainter::DrawMidBresenhamEllipse<GrayImage, uint8_t>(GrayImage &, float, float, float, float, const uint8_t &);
template void ImagePainter::DrawDashedLine<GrayImage, uint8_t>(GrayImage &, float, float, float, float, float, const uint8_t &);
template void ImagePainter::DrawCharacter<GrayImage, uint8_t>(GrayImage &, char, float, float, const uint8_t &, int32_t);
template void ImagePainter::DrawString<GrayImage, uint8_t>(GrayImage &, const std::string &, float, float, const uint8_t &, int32_t);
template void ImagePainter::DrawTrustRegionOfGaussian<GrayImage, uint8_t>(GrayImage &, float, float, const Mat2 &, const uint8_t &, float);
template void ImagePainter::DrawSolidRectangle<RgbImage, RgbPixel>(RgbImage &, float, float, float, float, const RgbPixel &);
template void ImagePainter::DrawHollowRectangle<RgbImage, RgbPixel>(RgbImage &, float, float, float, float, const RgbPixel &);
template void ImagePainter::DrawBressenhanLine<RgbImage, RgbPixel>(RgbImage &, float, float, float, float, const RgbPixel &);
template void ImagePainter::DrawNaiveLine<RgbImage, RgbPixel>(RgbImage &, float, float, float, float, const RgbPixel &);
template void ImagePainter::DrawSolidCircle<RgbImage, RgbPixel>(RgbImage &, float, float, float, const RgbPixel &);
template void ImagePainter::DrawHollowCircle<RgbImage, RgbPixel>(RgbImage &, float, float, float, const RgbPixel &);
template void ImagePainter::DrawMidBresenhamEllipse<RgbImage, RgbPixel>(RgbImage &, float, float, float, float, const RgbPixel &);
template void ImagePainter::DrawDashedLine<RgbImage, RgbPixel>(RgbImage &, float, float, float, float, float, const RgbPixel &);
template void ImagePainter::DrawCharacter<RgbImage, RgbPixel>(RgbImage &, char, float, float, const RgbPixel &, int32_t);
template void ImagePainter::DrawString<RgbImage, RgbPixel>(RgbImage &, const std::string &, float, float, const RgbPixel &, int32_t);
template void ImagePainter::DrawTrustRegionOfGaussian<RgbImage, RgbPixel>(RgbImage &, float, float, const Mat2 &, const RgbPixel &, float);

}  // namespace image_painter
