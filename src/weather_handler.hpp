#ifndef _WEATHER_HANDLER_HPP_
#define _WEATHER_HANDLER_HPP_

#include "api_handler.hpp"
#include "json.hpp"

namespace WeatherHandler
{
    /**
     * @brief Get the Weather String objects
     * 
     * @param place 
     * @param urlResolver 
     * @return std::string 
     */
    std::string GetWeatherString(const Place &place, const ApiHandler &urlResolver)
    {
        auto wUrl = urlResolver.getUrl<OpenWeatherApiRequestTypes::kCurrentWeather, Place>(place);
        auto fwUrl = urlResolver.getUrl<OpenWeatherApiRequestTypes::kForecastWeather, Place>(place);
        std::vector<std::string> urls{wUrl, fwUrl};
        auto w = ApiHandler::MakeRequests(urls);
        try
        {
            Json::json cwStrjson = Json::json::parse(std::string(w[0].begin(), w[0].end()));
            auto currentWeather = Json::GetData<Json::CurrentWeather>(cwStrjson);
            Json::json fwStrjson = Json::json::parse(std::string(w[1].begin(), w[1].end()));
            auto forecastWeather = Json::GetData<Json::Forecast>(fwStrjson);
            std::stringstream ss;
            ss << "Today: " << std::setprecision(3) << currentWeather.main.temp_max - 273.0 << " Tomorrow: " << forecastWeather.forecast[0].main.temp_max - 273.0;
            Logger::Print(Logger::Level::DEBUG, "String to add on the image: " + ss.str());
            return ss.str();
        }
        catch (...)
        {
            return "";
        }
    };
};

#endif