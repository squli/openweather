#ifndef _MAPS_HANDLER_HPP_
#define _MAPS_HANDLER_HPP_

#include "api_handler.hpp"
#include <opencv2/opencv.hpp>

/**
 * @brief 
 * 
 */
class MapsHandler
{
public:
    MapsHandler(const Place &place, const ApiHandler &urlResolver, WeatherMapSettings::OpenWeatherApiRequestTypesEnum wMapType) : 
    _place(place), _apiHandler(urlResolver), _type(wMapType) {};

    /**
     * @brief Get the Full Map object
     * 
     * @param res 
     * @return true 
     * @return false 
     */
    bool GetFullMap(cv::Mat &res) const;

    /**
     * @brief 
     * 
     * @param text 
     * @param dst 
     */
    static void addText(const std::string &text, cv::Mat &dst);

    /**
     * @brief Get the Map object
     * 
     * @param urls 
     * @return cv::Mat 
     */
    static cv::Mat GetMap(const std::vector<std::string> &urls);

    /**
     * @brief 
     * 
     * @param imgs 
     * @return cv::Mat 
     */
    static cv::Mat CombineImages(const std::vector<std::vector<uint8_t>> &imgs);

    /**
     * @brief 
     * 
     * @param start 
     * @return cv::Mat 
     */
    static cv::Mat VerticalConcat(std::vector<cv::Mat>::const_iterator start);

private:
    std::pair<int, int> GetTile() const;

    const Place &_place;
    const ApiHandler &_apiHandler;
    WeatherMapSettings::OpenWeatherApiRequestTypesEnum _type;

    static constexpr uint8_t kMapScaleLevel = 8U;
};

#endif