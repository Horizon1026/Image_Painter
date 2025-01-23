#include "image_painter.h"
#include "assic_fonts.h"

#include "slam_log_reporter.h"
#include "slam_memory.h"
#include "slam_operations.h"

namespace IMAGE_PAINTER {

namespace {
    constexpr float kMinValidViewDepth = 0.1f;
}

template void ImagePainter::RenderTextInCameraView<GrayImage, uint8_t>(GrayImage &image, const CameraView &cam, const Vec3 &p_w, const std::string &str, const uint8_t color, const int32_t font_size);
template void ImagePainter::RenderTextInCameraView<RgbImage, RgbPixel>(RgbImage &image, const CameraView &cam, const Vec3 &p_w, const std::string &str, const RgbPixel color, const int32_t font_size);
template <typename ImageType, typename PixelType>
void ImagePainter::RenderTextInCameraView(ImageType &image, const CameraView &cam, const Vec3 &p_w, const std::string &str, const PixelType color, const int32_t font_size) {
    const Vec3 p_c = cam.q_wc.inverse() * (p_w - cam.p_wc);
    RETURN_IF(p_c.z() < kMinValidViewDepth);
    const Vec2 pixel_uv_float = Vec2(p_c.x() / p_c.z() * cam.fx + cam.cx, p_c.y() / p_c.z() * cam.fy + cam.cy);
    const Pixel pixel_uv = pixel_uv_float.cast<int32_t>();
    DrawString(image, str, pixel_uv.x(), pixel_uv.y(), color, font_size);
}

template void ImagePainter::RenderPointInCameraView<GrayImage, uint8_t>(GrayImage &image, const CameraView &cam, const Vec3 &point_in_w, const uint8_t color, const int32_t radius);
template void ImagePainter::RenderPointInCameraView<RgbImage, RgbPixel>(RgbImage &image, const CameraView &cam, const Vec3 &point_in_w, const RgbPixel color, const int32_t radius);
template <typename ImageType, typename PixelType>
void ImagePainter::RenderPointInCameraView(ImageType &image, const CameraView &cam, const Vec3 &point_in_w, const PixelType color, const int32_t radius) {
    const Vec3 p_c = cam.q_wc.inverse() * (point_in_w - cam.p_wc);
    RETURN_IF(p_c.z() < kMinValidViewDepth);
    const Vec2 pixel_uv_float = Vec2(p_c.x() / p_c.z() * cam.fx + cam.cx, p_c.y() / p_c.z() * cam.fy + cam.cy);
    const Pixel pixel_uv = pixel_uv_float.cast<int32_t>();
    DrawSolidCircle(image, pixel_uv.x(), pixel_uv.y(), radius, color);
}

template void ImagePainter::RenderLineSegmentInCameraView<GrayImage, uint8_t>(GrayImage &image, const CameraView &cam, const Vec3 &line_s_point, const Vec3 &line_e_point, const uint8_t color);
template void ImagePainter::RenderLineSegmentInCameraView<RgbImage, RgbPixel>(RgbImage &image, const CameraView &cam, const Vec3 &line_s_point, const Vec3 &line_e_point, const RgbPixel color);
template <typename ImageType, typename PixelType>
void ImagePainter::RenderLineSegmentInCameraView(ImageType &image, const CameraView &cam, const Vec3 &line_s_point, const Vec3 &line_e_point, const PixelType color) {
    Vec3 p_c_i = cam.q_wc.inverse() * (line_s_point - cam.p_wc);
    Vec3 p_c_j = cam.q_wc.inverse() * (line_e_point - cam.p_wc);
    RETURN_IF(p_c_i.z() < kMinValidViewDepth && p_c_j.z() < kMinValidViewDepth);

    // If one point of line is outside, cut this line to make the two points of new line all visilbe.
    if (p_c_i.z() < kMinValidViewDepth || p_c_j.z() < kMinValidViewDepth) {
        const float w = (p_c_i.z() - kMinValidViewDepth) / (p_c_i.z() - p_c_j.z());
        const Vec3 p_c_mid = Vec3(w * p_c_j.x() + (1.0f - w) * p_c_i.x(),
                                  w * p_c_j.y() + (1.0f - w) * p_c_i.y(),
                                  w * p_c_j.z() + (1.0f - w) * p_c_i.z());
        if (p_c_i.z() < kMinValidViewDepth) {
            p_c_i = p_c_mid;
        } else {
            p_c_j = p_c_mid;
        }
    }

    const Pixel pixel_uv_i = Vec2(p_c_i.x() / p_c_i.z() * cam.fx + cam.cx, p_c_i.y() / p_c_i.z() * cam.fy + cam.cy).cast<int32_t>();
    const Pixel pixel_uv_j = Vec2(p_c_j.x() / p_c_j.z() * cam.fx + cam.cx, p_c_j.y() / p_c_j.z() * cam.fy + cam.cy).cast<int32_t>();
    DrawBressenhanLine(image, pixel_uv_i.x(), pixel_uv_i.y(), pixel_uv_j.x(), pixel_uv_j.y(), color);
}

template void ImagePainter::RenderDashedLineSegmentInCameraView<GrayImage, uint8_t>(GrayImage &image, const CameraView &cam, const Vec3 &line_s_point, const Vec3 &line_e_point, const int32_t dot_step, const uint8_t color);
template void ImagePainter::RenderDashedLineSegmentInCameraView<RgbImage, RgbPixel>(RgbImage &image, const CameraView &cam, const Vec3 &line_s_point, const Vec3 &line_e_point, const int32_t dot_step, const RgbPixel color);
template <typename ImageType, typename PixelType>
void ImagePainter::RenderDashedLineSegmentInCameraView(ImageType &image, const CameraView &cam, const Vec3 &line_s_point, const Vec3 &line_e_point, const int32_t dot_step, const PixelType color) {
    Vec3 p_c_i = cam.q_wc.inverse() * (line_s_point - cam.p_wc);
    Vec3 p_c_j = cam.q_wc.inverse() * (line_e_point - cam.p_wc);
    RETURN_IF(p_c_i.z() < kMinValidViewDepth && p_c_j.z() < kMinValidViewDepth);

    // If one point of line is outside, cut this line to make the two points of new line all visilbe.
    if (p_c_i.z() < kMinValidViewDepth || p_c_j.z() < kMinValidViewDepth) {
        const float w = (p_c_i.z() - kMinValidViewDepth) / (p_c_i.z() - p_c_j.z());
        const Vec3 p_c_mid = Vec3(w * p_c_j.x() + (1.0f - w) * p_c_i.x(),
                                  w * p_c_j.y() + (1.0f - w) * p_c_i.y(),
                                  w * p_c_j.z() + (1.0f - w) * p_c_i.z());
        if (p_c_i.z() < kMinValidViewDepth) {
            p_c_i = p_c_mid;
        } else {
            p_c_j = p_c_mid;
        }
    }

    const Pixel pixel_uv_i = Vec2(p_c_i.x() / p_c_i.z() * cam.fx + cam.cx, p_c_i.y() / p_c_i.z() * cam.fy + cam.cy).cast<int32_t>();
    const Pixel pixel_uv_j = Vec2(p_c_j.x() / p_c_j.z() * cam.fx + cam.cx, p_c_j.y() / p_c_j.z() * cam.fy + cam.cy).cast<int32_t>();
    DrawDashedLine(image, pixel_uv_i.x(), pixel_uv_i.y(), pixel_uv_j.x(), pixel_uv_j.y(), dot_step, color);
}

template void ImagePainter::RenderEllipseInCameraView<GrayImage, uint8_t>(GrayImage &image, const CameraView &cam, const Vec3 &mid_p_w, const Mat3 &covariance, const uint8_t color);
template void ImagePainter::RenderEllipseInCameraView<RgbImage, RgbPixel>(RgbImage &image, const CameraView &cam, const Vec3 &mid_p_w, const Mat3 &covariance, const RgbPixel color);
template <typename ImageType, typename PixelType>
void ImagePainter::RenderEllipseInCameraView(ImageType &image, const CameraView &cam, const Vec3 &mid_p_w, const Mat3 &covariance, const PixelType color) {
    // Transform gaussian ellipse into camera frame.
    const Vec3 p_c = cam.q_wc.inverse() * (mid_p_w - cam.p_wc);
    const Mat3 cov_c = cam.q_wc.inverse() * covariance * cam.q_wc;
    RETURN_IF(p_c.z() < kMinValidViewDepth);

    // Compute focus of camera.
    const float focus = 0.5f * (cam.fx + cam.fy);

    // Transform 3d gaussian into 2d gaussian.
    const float inv_depth = 1.0f / p_c.z();
    const float inv_depth_2 = inv_depth * inv_depth;
    Mat2x3 jacobian_2d_3d = Mat2x3::Zero();
    if (!std::isnan(inv_depth)) {
        jacobian_2d_3d << inv_depth, 0, - p_c(0) * inv_depth_2,
                          0, inv_depth, - p_c(1) * inv_depth_2;
        jacobian_2d_3d = jacobian_2d_3d * focus;
    }
    const Mat2 pixel_cov = jacobian_2d_3d * cov_c * jacobian_2d_3d.transpose();
    const Vec2 pixel_uv = p_c.head<2>() * inv_depth * focus + Vec2(cam.cx, cam.cy);

    // Draw boundary of 2d gaussian ellipse.
    DrawTrustRegionOfGaussian(image, pixel_uv, pixel_cov, color);
}

}
