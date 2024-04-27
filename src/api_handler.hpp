#ifndef _API_HANDLER_HPP_
#define _API_HANDLER_HPP_

/**
    @brief Form en url for request

    current:
   api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&appid={API key}
    forecast:
   api.openweathermap.org/data/2.5/forecast?lat={lat}&lon={lon}&appid={API key}
    geocoding: api.openweathermap.org/geo/1.0/direct?q={city name},{state
   code},{country code}&limit={limit}&appid={API key}
                api.openweathermap.org/geo/1.0/direct?q=London&limit=5&appid={API
   key} maps: tile.openweathermap.org/map/{layer}/{z}/{x}/{y}.png?appid={API
   key} raw maps: https://tile.openstreetmap.org/8/145/74.png
*/

#include <iostream>
#include <vector>
#include <map>
#include <string_view>
#include <sstream>
#include <iomanip>
#include <future>
#include "curl/curl.h"
#include "logger.hpp"
#include <fstream>
#include "json.hpp"

struct WeatherMapSettings
{
    enum class OpenWeatherApiRequestTypesEnum
    {
        kCloudsNew = 0,
        kPrecipitationNew,
        kPressureNew,
        kWindNew,
        kTempNew,

        kCount,
    };

    WeatherMapSettings(WeatherMapSettings::OpenWeatherApiRequestTypesEnum t, size_t z, int x, int y) : type(t), z(z), x(x), y(y){};

    static inline std::map<WeatherMapSettings::OpenWeatherApiRequestTypesEnum, std::string> OpenWeatherMapRequestTypes{
        {WeatherMapSettings::OpenWeatherApiRequestTypesEnum::kCloudsNew, "clouds_new"},
        {WeatherMapSettings::OpenWeatherApiRequestTypesEnum::kPrecipitationNew, "precipitation_new"},
        {WeatherMapSettings::OpenWeatherApiRequestTypesEnum::kPressureNew, "pressure_new"},
        {WeatherMapSettings::OpenWeatherApiRequestTypesEnum::kTempNew, "temp_new"},
        {WeatherMapSettings::OpenWeatherApiRequestTypesEnum::kWindNew, "wind_new"},
    };

    WeatherMapSettings::OpenWeatherApiRequestTypesEnum type;
    size_t z;
    int x;
    int y;

    std::string toString() const
    {
        std::stringstream ss;
        ss << OpenWeatherMapRequestTypes[type] << "/" << z << "/" << x << "/" << y << ".png";
        return ss.str();
    }

    std::string toStringNoType() const
    {
        std::stringstream ss;
        ss << z << "/" << x << "/" << y << ".png";
        return ss.str();
    }
};

enum class OpenWeatherApiRequestTypes
{
    kCurrentWeather = 0,
    kForecastWeather,
    kGeocoding,
    kWeatherMaps,
    kMaps,

    kCount,
};

class ApiHandler
{
public:
    ApiHandler(const std::string_view &apiKey) : _apiKey(apiKey){};

    /**
     * @brief Get the Place object
     *
     * @param city
     * @param place
     * @return true
     * @return false
     */
    bool GetPlace(const std::string &city, Place &place);

    /**
     * @brief Get the Url object
     *
     * @tparam requestType
     * @tparam requestParam
     * @param param
     * @return std::string
     */
    template <OpenWeatherApiRequestTypes requestType, typename requestParam>
    std::string getUrl(const requestParam &param) const
    {
        std::ostringstream ostream;

        if constexpr (requestType == OpenWeatherApiRequestTypes::kCurrentWeather &&
                      std::is_same_v<requestParam, Place>)
        {
            ostream << kUrlBasePathV2 << "weather?" << param.toString() << "&";
        }
        else if constexpr (requestType ==
                               OpenWeatherApiRequestTypes::kForecastWeather &&
                           std::is_same_v<requestParam, Place>)
        {
            ostream << kUrlBasePathV2 << "forecast?" << param.toString() << "&";
        }
        else if constexpr (requestType == OpenWeatherApiRequestTypes::kGeocoding &&
                           std::is_same_v<requestParam, std::string>)
        {
            ostream << kUrlBasePathV1 << "direct?q=" << param << "&";
        }
        else if constexpr (requestType ==
                               OpenWeatherApiRequestTypes::kWeatherMaps &&
                           std::is_same_v<requestParam, WeatherMapSettings>)
        {
            ostream << kUrlBasePathMap << param.toString() << "?";
        }
        else if constexpr (requestType == OpenWeatherApiRequestTypes::kMaps &&
                           std::is_same_v<requestParam, WeatherMapSettings>)
        {
            ostream << kUrlBasePathPhysMap << param.toStringNoType();
            return ostream.str();
        }
        else
        {
            static_assert(0);
        }
        ostream << std::string{"appid="} << _apiKey;

        return ostream.str();
    }

    /**
     * @brief
     *
     * @param weatherMapSettings
     * @return std::pair<std::vector<std::string>, std::vector<std::string>>
     */
    std::pair<std::vector<std::string>, std::vector<std::string>> MakeVectorOfMapUrls(const WeatherMapSettings &weatherMapSettings) const;

    /**
     * @brief
     *
     * @return std::vector<std::vector<uint8_t> >
     */
    static std::vector<std::vector<uint8_t>> MakeRequests(std::vector<std::string> url);

private:
    /**
     * @brief
     *
     * @param url
     * @return std::string
     */
    std::string makeSingleRequest(const std::string &url);

    const std::string_view _apiKey;

    static constexpr std::string_view kUrlBasePathV1{"api.openweathermap.org/geo/1.0/"};
    static constexpr std::string_view kUrlBasePathV2{"api.openweathermap.org/data/2.5/"};
    static constexpr std::string_view kUrlBasePathMap{"tile.openweathermap.org/map/"};
    static constexpr std::string_view kUrlBasePathPhysMap{"tile.openstreetmap.org/"};
};

#endif