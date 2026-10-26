#include "image_painter.h"

#include "basic_type.h"
#include "slam_log_reporter.h"
#include "slam_memory.h"
#include "visualizor_2d.h"

using namespace image_painter;
using namespace slam_utility;
using namespace slam_visualizor;

namespace {
constexpr int32_t kScale = 3;
constexpr int32_t kMatrixRow = 90;
constexpr int32_t kMatrixCol = 180;
}  // namespace

int main(int argc, char **argv) {
    ReportInfo(YELLOW ">> Test image painter." << RESET_COLOR);
    const std::string png_image_file = "../example/image.png";

    // Create image of matrix.
    Mat matrix = Mat::Identity(kMatrixRow, kMatrixCol) * 10.0f;
    matrix += Mat::Random(kMatrixRow, kMatrixCol);
    uint8_t *buf = (uint8_t *)malloc(matrix.rows() * matrix.cols() * kScale * kScale * sizeof(uint8_t));
    GrayImage image_matrix(buf, matrix.rows() * kScale, matrix.cols() * kScale, true);
    ImagePainter::ConvertMatrixToImage<float>(matrix, image_matrix, 15.0f, kScale);
    ImagePainter::DrawSolidRectangle(image_matrix, 10, 10, 200, 200, static_cast<uint8_t>(0));
    for (int32_t i = 0; i < 10; ++i) {
        ImagePainter::DrawBressenhanLine(image_matrix, 111 - 10 * i, 10 * i, 100, 100, static_cast<uint8_t>(255));
    }
    ImagePainter::DrawSolidCircle(image_matrix, 130, 200, 10, static_cast<uint8_t>(127));
    ImagePainter::DrawString(image_matrix, "This is a string.", 240, 100 - 16, static_cast<uint8_t>(0), 99);
    ImagePainter::DrawString(image_matrix, "This is a string.", 240, 100, static_cast<uint8_t>(127), 16);
    ImagePainter::DrawMidBresenhamEllipse(image_matrix, 180, 80, 40, 20, static_cast<uint8_t>(127));
    ImagePainter::DrawDashedLine(image_matrix, 20, 20, 60, 80, 5, static_cast<uint8_t>(200));

    // Create image of matrix with subpixel painting.
    uint8_t *subpixel_buf = (uint8_t *)malloc(matrix.rows() * matrix.cols() * kScale * kScale * sizeof(uint8_t));
    GrayImage image_matrix_subpixel(subpixel_buf, matrix.rows() * kScale, matrix.cols() * kScale, true);
    ImagePainter::ConvertMatrixToImage<float>(matrix, image_matrix_subpixel, 15.0f, kScale);
    ImagePainter::DrawSolidRectangle(image_matrix_subpixel, 10.25f, 10.5f, 200.0f, 200.0f, static_cast<uint8_t>(0));
    ImagePainter::DrawHollowRectangle(image_matrix_subpixel, 20.25f, 20.5f, 180.0f, 180.0f, static_cast<uint8_t>(127));
    for (int32_t i = 0; i < 10; ++i) {
        ImagePainter::DrawBressenhanLine(image_matrix_subpixel, 111.25f - 10.0f * i, 10.5f * i, 100.25f, 100.5f, static_cast<uint8_t>(255));
    }
    ImagePainter::DrawNaiveLine(image_matrix_subpixel, 20.25f, 40.5f, 180.25f, 100.5f, static_cast<uint8_t>(200));
    ImagePainter::DrawSolidCircle(image_matrix_subpixel, 130.25f, 200.5f, 10.0f, static_cast<uint8_t>(127));
    ImagePainter::DrawHollowCircle(image_matrix_subpixel, 160.25f, 200.5f, 10.0f, static_cast<uint8_t>(200));
    ImagePainter::DrawString(image_matrix_subpixel, "This is a string.", 240.25f, 100.5f - 16.0f, static_cast<uint8_t>(0), 99);
    ImagePainter::DrawString(image_matrix_subpixel, "This is a string.", 240.25f, 100.5f, static_cast<uint8_t>(127), 16);
    ImagePainter::DrawCharacter(image_matrix_subpixel, 'A', 240.25f, 130.5f, static_cast<uint8_t>(127), 24);
    ImagePainter::DrawMidBresenhamEllipse(image_matrix_subpixel, 180.25f, 80.5f, 40.0f, 20.0f, static_cast<uint8_t>(127));
    ImagePainter::DrawDashedLine(image_matrix_subpixel, 20.25f, 20.5f, 60.25f, 80.5f, 5.0f, static_cast<uint8_t>(200));

    // Create image of png file.
    RgbImage rgb_image_png;
    Visualizor2D::LoadImage(png_image_file, rgb_image_png);
    ImagePainter::DrawHollowRectangle(rgb_image_png, 20, 20, 200, 200, RgbColor::kYellow);
    for (int32_t i = 0; i < 10; ++i) {
        ImagePainter::DrawNaiveLine(rgb_image_png, 111 - 10 * i, 10 * i, 100, 100, RgbColor::kGreen);
    }
    ImagePainter::DrawHollowCircle(rgb_image_png, 130, 200, 10, RgbColor::kBlue);
    ImagePainter::DrawString(rgb_image_png, "This is a string.", 0, 0, RgbColor::kYellow, 24);
    ImagePainter::DrawMidBresenhamEllipse(rgb_image_png, 200, 200, 20, 70, RgbColor::kOrangeRed);

    // Create image of png file with subpixel painting.
    RgbImage rgb_image_png_subpixel;
    Visualizor2D::LoadImage(png_image_file, rgb_image_png_subpixel);
    ImagePainter::DrawSolidRectangle(rgb_image_png_subpixel, 30.25f, 30.5f, 60.0f, 40.0f, RgbColor::kBlue);
    ImagePainter::DrawHollowRectangle(rgb_image_png_subpixel, 20.25f, 20.5f, 200.0f, 200.0f, RgbColor::kYellow);
    for (int32_t i = 0; i < 10; ++i) {
        ImagePainter::DrawNaiveLine(rgb_image_png_subpixel, 111.25f - 10.0f * i, 10.5f * i, 100.25f, 100.5f, RgbColor::kGreen);
    }
    ImagePainter::DrawBressenhanLine(rgb_image_png_subpixel, 20.25f, 40.5f, 180.25f, 100.5f, RgbColor::kYellow);
    ImagePainter::DrawSolidCircle(rgb_image_png_subpixel, 160.25f, 200.5f, 10.0f, RgbColor::kGreen);
    ImagePainter::DrawHollowCircle(rgb_image_png_subpixel, 130.25f, 200.5f, 10.0f, RgbColor::kBlue);
    ImagePainter::DrawString(rgb_image_png_subpixel, "This is a string.", 0.25f, 0.5f, RgbColor::kYellow, 24);
    ImagePainter::DrawCharacter(rgb_image_png_subpixel, 'A', 240.25f, 40.5f, RgbColor::kYellow, 16);
    ImagePainter::DrawMidBresenhamEllipse(rgb_image_png_subpixel, 200.25f, 200.5f, 20.0f, 70.0f, RgbColor::kOrangeRed);
    ImagePainter::DrawDashedLine(rgb_image_png_subpixel, 20.25f, 20.5f, 60.25f, 80.5f, 5.0f, RgbColor::kYellow);

    // Draw ellipses and rectangle to check the size of ellipses.
    Mat2 cov = Mat2::Identity();
    cov << 20 * 20, 0, 0, 70 * 70;
    for (float angle = 0.0f; angle < 3.14f; angle += 0.785f) {
        Mat2 rot = Mat2::Zero();
        rot << std::cos(angle), -std::sin(angle), std::sin(angle), std::cos(angle);
        if (angle > 1.0) {
            ImagePainter::DrawTrustRegionOfGaussian(rgb_image_png, 350, 200, rot * cov * rot.transpose(), RgbColor::kLaunGreen, 1.0f);
            ImagePainter::DrawTrustRegionOfGaussian(rgb_image_png_subpixel, 350.25f, 200.5f, rot * cov * rot.transpose(), RgbColor::kLaunGreen, 1.0f);
        } else {
            ImagePainter::DrawTrustRegionOfGaussian(rgb_image_png, 350, 200, rot * cov * rot.transpose(), RgbColor::kViolet, 1.0f);
            ImagePainter::DrawTrustRegionOfGaussian(rgb_image_png_subpixel, 350.25f, 200.5f, rot * cov * rot.transpose(), RgbColor::kViolet, 1.0f);
        }
        ImagePainter::DrawTrustRegionOfGaussian(image_matrix_subpixel, 350.25f, 200.5f, rot * cov * rot.transpose(), static_cast<uint8_t>(127), 1.0f);
    }
    ImagePainter::DrawHollowRectangle(rgb_image_png, 300, 300, 20, 70, RgbColor::kYellow);
    ImagePainter::DrawHollowRectangle(rgb_image_png_subpixel, 300.25f, 300.5f, 20.0f, 70.0f, RgbColor::kYellow);

    // Show painted image.
    Visualizor2D::ShowImage("Matrix image", image_matrix);
    Visualizor2D::WaitKey(1);
    Visualizor2D::ShowImage("Rgb Png Image", rgb_image_png);
    Visualizor2D::WaitKey(1);
    Visualizor2D::ShowImage("Subpixel Matrix Image", image_matrix_subpixel);
    Visualizor2D::WaitKey(1);
    Visualizor2D::ShowImage("Subpixel Rgb Png Image", rgb_image_png_subpixel);
    Visualizor2D::WaitKey(0);

    return 0;
}
