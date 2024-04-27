#include "maps_handler.hpp"
#include <limits>
#include <numbers>

bool MapsHandler::GetFullMap(cv::Mat &res) const
{
    const auto x_y = GetTile();
    
    Logger::Print(Logger::Level::DEBUG, "Getting tiles done: " + std::to_string(x_y.first) + "/" + std::to_string(x_y.second));

    WeatherMapSettings weatherMapSettings{_type, MapsHandler::kMapScaleLevel, x_y.first, x_y.second};
    auto urls = _apiHandler.MakeVectorOfMapUrls(weatherMapSettings);
    std::vector<std::string> phUrls = urls.second;
    std::vector<std::string> wUrls = urls.first;

    if (Logger::verbose) {
        Logger::Print(Logger::Level::DEBUG, "Urls to get maps: ");
        for (auto &s: phUrls) {
            Logger::Print(Logger::Level::DEBUG, s);
        }
        for (auto &s: wUrls) {
            Logger::Print(Logger::Level::DEBUG, s);
        }
    }

    Logger::Print(Logger::Level::DEBUG, "Start map requests");
    auto wMap(std::async(std::launch::async, MapsHandler::GetMap, wUrls));
    auto phMap(std::async(std::launch::async, MapsHandler::GetMap, phUrls));
    auto combinedPMap = phMap.get();
    cv::cvtColor(combinedPMap, combinedPMap, cv::COLOR_BGR2BGRA, 4);

    Logger::Print(Logger::Level::DEBUG, "Combine physical and weather images");
    cv::addWeighted(wMap.get(), 0.45, combinedPMap, 0.52, 0.0, res);

    return true;
};

void MapsHandler::addText(const std::string &text, cv::Mat &dst)
{
    // Font properties
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 1;
    cv::Scalar color = cv::Scalar(255, 255, 255); // White color
    int thickness = 2;

    // Position where the text will be added
    cv::Point textOrg(10, dst.rows - 10); // Bottom-left corner of the text

    // Add text to the image
    cv::putText(dst, text, textOrg, fontFace, fontScale, color, thickness);
};

cv::Mat MapsHandler::GetMap(const std::vector<std::string> &urls)
{
    std::vector<std::vector<uint8_t>> maps = ApiHandler::MakeRequests(urls);
    return MapsHandler::CombineImages(maps);
};

cv::Mat MapsHandler::CombineImages(const std::vector<std::vector<uint8_t>> &imgs)
{
    std::vector<cv::Mat> images;
    for (const std::vector<uint8_t> &img : imgs)
    {
        images.push_back(cv::imdecode(img, cv::IMREAD_UNCHANGED));
    }

    auto combinedImageCenterColumn = VerticalConcat(images.begin());
    auto combinedImageLeftColumn = VerticalConcat(images.begin() + 3);
    auto combinedImageRightColumn = VerticalConcat(images.begin() + 6);

    cv::Mat combinedImage;
    cv::hconcat(combinedImageLeftColumn, combinedImageCenterColumn, combinedImage);
    cv::hconcat(combinedImage, combinedImageRightColumn, combinedImage);

    if (Logger::verbose)
    {
        cv::imwrite("combinedImageCenterColumn.png", combinedImageCenterColumn);
        cv::imwrite("combinedImageLeftColumn.png", combinedImageLeftColumn);
        cv::imwrite("combinedImageRightColumn.png", combinedImageRightColumn);
        cv::imwrite("combinedImage.png", combinedImage);
    }

    return combinedImage;
};

cv::Mat MapsHandler::VerticalConcat(std::vector<cv::Mat>::const_iterator start)
{
    cv::Mat combinedImage;
    cv::vconcat(*(start + 1), *start, combinedImage);
    cv::vconcat(combinedImage, *(start + 2), combinedImage);
    return combinedImage;
}

std::pair<int, int> MapsHandler::GetTile() const
{
    constexpr double pi{3.14};
    float lat_deg = _place.lat;
    float lon_deg = _place.lon;
    double lat_rad = lat_deg * (pi / 180);
    int n = pow(2, MapsHandler::kMapScaleLevel);
    int xtile = static_cast<int>((lon_deg + 180.0) / 360.0 * n);
    int ytile = static_cast<int>((1.0 - std::asinh(std::tan(lat_rad)) / pi) / 2.0 * n);
    return std::pair<int, int>(xtile, ytile);
}
