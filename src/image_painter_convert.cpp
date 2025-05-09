#include "image_painter.h"

#include "slam_log_reporter.h"
#include "slam_memory.h"
#include "slam_operations.h"

namespace IMAGE_PAINTER {

void ImagePainter::ConvertUint8ToRgb(const uint8_t *gray, uint8_t *rgb, int32_t gray_size) {
    for (int32_t i = 0; i < gray_size; ++i) {
        const int32_t idx = i * 3;
        std::fill_n(rgb + idx, 3, gray[i]);
    }
}

void ImagePainter::ConvertRgbToUint8(const uint8_t *rgb, uint8_t *gray, int32_t gray_size) {
    for (int32_t i = 0; i < gray_size; ++i) {
        const int32_t idx = i * 3;
        gray[i] = static_cast<uint8_t>(static_cast<float>(rgb[idx]) * 0.299f +
                                       static_cast<float>(rgb[idx + 1]) * 0.587f +
                                       static_cast<float>(rgb[idx + 2]) * 0.114f);
    }
}

void ImagePainter::ConvertUint8ToRgbAndUpsideDown(const uint8_t *gray,
                                                  uint8_t *rgb,
                                                  int32_t gray_rows,
                                                  int32_t gray_cols) {
    const int32_t gray_cols_3 = 3 * gray_cols;

    for (int32_t row = 0; row < gray_rows; ++row) {
        for (int32_t col = 0; col < gray_cols; ++col) {
            const int32_t offset = (gray_rows - row - 1) * gray_cols_3 + 3 * col;
            std::fill_n(rgb + offset, 3, gray[col + row * gray_cols]);
        }
    }
}

void ImagePainter::ConvertRgbToBgr(const uint8_t *rgb,
                                   uint8_t *converted_rgb,
                                   int32_t rgb_rows,
                                   int32_t rgb_cols) {
    const int32_t rgb_stride = rgb_cols * 3;
    for (int32_t row = 0; row < rgb_rows; ++row) {
        for (int32_t col = 0; col < rgb_cols; ++col) {
            const int32_t offset_col = 3 * col;
            const int32_t offset = row * rgb_stride + offset_col;
            converted_rgb[offset] = rgb[offset + 2];
            converted_rgb[offset + 1] = rgb[offset + 1];
            converted_rgb[offset + 2] = rgb[offset];
        }
    }
}

void ImagePainter::ConvertRgbToBgrAndUpsideDown(const uint8_t *rgb,
                                                uint8_t *converted_rgb,
                                                int32_t rgb_rows,
                                                int32_t rgb_cols) {
    const int32_t rgb_stride = rgb_cols * 3;
    for (int32_t row = 0; row < rgb_rows; ++row) {
        for (int32_t col = 0; col < rgb_cols; ++col) {
            const int32_t offset_col = 3 * col;
            const int32_t offset = row * rgb_stride + offset_col;
            const int32_t offset_converted = (rgb_rows - row - 1) * rgb_stride + offset_col;
            converted_rgb[offset_converted] = rgb[offset + 2];
            converted_rgb[offset_converted + 1] = rgb[offset + 1];
            converted_rgb[offset_converted + 2] = rgb[offset];
        }
    }
}

template uint8_t ImagePainter::ConvertValueToUint8<float>(float value, float max_value);
template uint8_t ImagePainter::ConvertValueToUint8<double>(double value, double max_value);
template <typename Scalar>
uint8_t ImagePainter::ConvertValueToUint8(Scalar value, Scalar max_value) {
    value = std::fabs(value);
    if (value >= max_value) {
        return 0;
    }

    const Scalar step = max_value / 256.0;
    return 255 - static_cast<uint8_t>(value / step);
}

template bool ImagePainter::ConvertMatrixToImage<float>(const TMat<float> &matrix, GrayImage &image, float max_value, int32_t scale);
template bool ImagePainter::ConvertMatrixToImage<double>(const TMat<double> &matrix, GrayImage &image, double max_value, int32_t scale);
template <typename Scalar>
bool ImagePainter::ConvertMatrixToImage(const TMat<Scalar> &matrix,
                                      GrayImage &image,
                                      Scalar max_value,
                                      int32_t scale) {
    if (image.data() == nullptr) {
        ReportError("[ImagePainter] GrayImage buffer is empty.");
        return false;
    }
    if (scale < 0) {
        ReportError("[ImagePainter] Scale must larger than 0.");
        return false;
    }
    if (image.rows() != matrix.rows() * scale || image.cols() != matrix.cols() * scale) {
        ReportError("[ImagePainter] GrayImage buffer size does not match matrix size.");
        return false;
    }

    // Convert matrix to image.
    for (int32_t row = 0; row < matrix.rows(); ++row) {
        for (int32_t col = 0; col < matrix.cols(); ++col) {
            // Compute image value in the first line.
            const uint8_t image_value = ConvertValueToUint8(matrix(row, col), max_value);
            const int32_t image_row = row * scale;
            const int32_t image_col = col * scale;

            // Fill the block in image.
            for (int32_t i = 0; i < scale; ++i) {
                std::fill_n(image.data() + (image_row + i) * image.cols() + image_col, scale, image_value);
            }

            // Draw delete line if value is nan.
            if (std::isnan(matrix(row, col))) {
                for (int32_t i = 0; i < scale; ++i) {
                    *(image.data() + (image_row + i) * image.cols() + image_col + i) = 127;
                }
            }
        }
    }

    return true;
}

template bool ImagePainter::ConvertMatrixToImage<float>(const TMat<float> &matrix, RgbImage &image, float max_value, int32_t scale);
template bool ImagePainter::ConvertMatrixToImage<double>(const TMat<double> &matrix, RgbImage &image, double max_value, int32_t scale);
template <typename Scalar>
bool ImagePainter::ConvertMatrixToImage(const TMat<Scalar> &matrix,
                                      RgbImage &image,
                                      Scalar max_value,
                                      int32_t scale) {
    if (image.data() == nullptr) {
        ReportError("[ImagePainter] RgbImage buffer is empty.");
        return false;
    }
    if (scale < 0) {
        ReportError("[ImagePainter] Scale must larger than 0.");
        return false;
    }
    if (image.rows() != matrix.rows() * scale || image.cols() != matrix.cols() * scale) {
        ReportError("[ImagePainter] RgbImage buffer size does not match matrix size.");
        return false;
    }

    // Convert matrix to image.
    for (int32_t row = 0; row < matrix.rows(); ++row) {
        for (int32_t col = 0; col < matrix.cols(); ++col) {
            // Compute image value in the first line.
            const uint8_t image_value = ConvertValueToUint8(matrix(row, col), max_value);
            const int32_t image_row = row * scale;
            const int32_t image_col = col * scale * 3;

            // Fill the block in image.
            for (int32_t i = 0; i < scale; ++i) {
                std::fill_n(image.data() + (image_row + i) * image.cols() + image_col, scale * 3, image_value);
            }
        }
    }

    return true;
}

}
