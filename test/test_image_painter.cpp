#include "image_painter.h"
#include "log_report.h"
#include "slam_memory.h"

#include "datatype_basic.h"
#include "visualizor.h"

using namespace IMAGE_PAINTER;
using namespace SLAM_UTILITY;
using namespace SLAM_VISUALIZOR;

namespace {
    constexpr int32_t kScale = 3;
    constexpr int32_t kMatrixRow = 90;
    constexpr int32_t kMatrixCol = 180;
}

int main(int argc, char **argv) {
    ReportInfo(YELLOW ">> Test image painter." << RESET_COLOR);
    const std::string png_image_file = "../example/image.png";

    // Create image of matrix.
    Mat matrix = Mat::Identity(kMatrixRow, kMatrixCol) * 10.0f;
    matrix += Mat::Random(kMatrixRow, kMatrixCol);
    uint8_t *buf = (uint8_t *)malloc(matrix.rows() * matrix.cols() * kScale * kScale * sizeof(uint8_t));
    GrayImage image_matrix(buf, matrix.rows() * kScale, matrix.cols() * kScale, true);
    ImagePainter::ConvertMatrixToImage<float>(matrix, image_matrix, 15.0f, kScale);
    ImagePainter::DrawSolidRectangle(image_matrix, 10.1, 10.1, 200, 200, static_cast<uint8_t>(0));
    for (int32_t i = 0; i < 10; ++i) {
        ImagePainter::DrawBressenhanLine(image_matrix, 111 - 10 * i, 10 * i, 100, 100, static_cast<uint8_t>(255));
    }
    ImagePainter::DrawSolidCircle(image_matrix, 130, 200, 10, static_cast<uint8_t>(127));
    ImagePainter::DrawString(image_matrix, "This is a string.", 240, 100 - 16, static_cast<uint8_t>(0), 99);
    ImagePainter::DrawString(image_matrix, "This is a string.", 240, 100, static_cast<uint8_t>(127), 16);
    ImagePainter::DrawMidBresenhamEllipse(image_matrix, 180, 80, 40, 20, static_cast<uint8_t>(127));

    // Create image of png file.
    RgbImage rgb_image_png;
    Visualizor::LoadImage(png_image_file, rgb_image_png);
    ImagePainter::DrawHollowRectangle(rgb_image_png, 20, 20, 200, 200, RgbColor::kYellow);
    for (int32_t i = 0; i < 10; ++i) {
        ImagePainter::DrawNaiveLine(rgb_image_png, 111 - 10 * i, 10 * i, 100, 100, RgbColor::kGreen);
    }
    ImagePainter::DrawHollowCircle(rgb_image_png, 130, 200, 10, RgbColor::kBlue);
    ImagePainter::DrawString(rgb_image_png, "This is a string.", 0, 0, RgbColor::kYellow, 24);
    ImagePainter::DrawMidBresenhamEllipse(rgb_image_png, 200, 200, 20, 70, RgbColor::kOrangeRed);

    Mat2 cov = Mat2::Identity();
    cov << 20, 0, 0, 70;
    for (float angle = 0.0f; angle < 3.14f; angle += 0.785f) {
        Mat2 rot = Mat2::Zero();
        rot << std::cos(angle), - std::sin(angle), std::sin(angle), std::cos(angle);
        ImagePainter::DrawTrustRegionOfGaussian(rgb_image_png, Vec2(350, 200), rot * cov * rot.transpose(), RgbColor::kViolet);
    }

    // Show painted image.
    Visualizor::ShowImage("Matrix image", image_matrix);
    Visualizor::WaitKey(1);
    Visualizor::ShowImage("Rgb Png Image", rgb_image_png);
    Visualizor::WaitKey(0);

    return 0;
}
