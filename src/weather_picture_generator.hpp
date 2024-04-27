#ifndef _WEATHER_PICTURE_GENERATOR_
#define _WEATHER_PICTURE_GENERATOR_

#include <string>

#include "api_handler.hpp"
#include "maps_handler.hpp"
#include "weather_handler.hpp"

/**
 * @brief 
 * 
 */
class WeatherPictureGenerator {
public:
    enum class WeatherPictureGeneratorErrors {
        OK = 0,
        NOT_KNOWN_PLACE = 100,
        CANT_GET_WEATHER,
        CANT_GET_MAPS,
        CANT_SAVE_FILE,
    };

    WeatherPictureGenerator(const std::string_view &apiKey, const std::string &placeStr) :
         _apiKey(apiKey), _placeStr(placeStr) {
    };

    /**
     * @brief 
     * 
     * @return WeatherPictureGeneratorErrors 
     */
    WeatherPictureGeneratorErrors generatePicture() const {
        ApiHandler apiHandler(_apiKey);

        // request place coordinates
        Place place{0, 0};
        if (!apiHandler.GetPlace(_placeStr, place)) {
            return WeatherPictureGeneratorErrors::NOT_KNOWN_PLACE;
        }

        // start weather requests
        auto weatherString = (std::async(std::launch::async, WeatherHandler::GetWeatherString, place, apiHandler));

        // start map requests & merge maps into two ones
        MapsHandler mapsHandler(place, apiHandler, WeatherMapSettings::OpenWeatherApiRequestTypesEnum::kCloudsNew);
        cv::Mat fullMap;
        if (!mapsHandler.GetFullMap(fullMap)) {
            return WeatherPictureGeneratorErrors::CANT_GET_MAPS;
        }

        auto weather = weatherString.get();
        if (weather.size() == 0) {
            return WeatherPictureGeneratorErrors::CANT_GET_WEATHER;
        }
        mapsHandler.addText(weather, fullMap);

        if (!cv::imwrite(_placeStr + ".png", fullMap)) {
            return WeatherPictureGeneratorErrors::CANT_SAVE_FILE;
        }

        Logger::Print(Logger::Level::INFO, "Picture was generated");

        return WeatherPictureGeneratorErrors::OK;
    };

private:
    const std::string_view &_apiKey;
    const std::string &_placeStr;
};

#endif