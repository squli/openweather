#ifndef _JSON_HPP_
#define _JSON_HPP_

#include <optional>
#include <string>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "logger.hpp"

/**
 * @brief 
 * 
 */
struct Place
{
    Place(float lt, float ln) : lat(lt), lon(ln){};

    float lat;
    float lon;

    std::string toString() const
    {
        return "lat=" + std::to_string(lat) + "&lon=" + std::to_string(lon);
    }
};

/**
 * @brief 
 * 
 */
struct GeocodingResponse
{
    GeocodingResponse(std::string n, std::string c, Place p) : name(n), country(c), place(p){};

    std::string name;
    std::string country;
    Place place;

    std::string toString()
    {
        std::stringstream ss;
        ss << "Place " << name << " in country " << country
           << " has coordinates { " << place.toString() << " }" << std::endl;
        return ss.str();
    };
};

namespace Json
{

    using nlohmann::json;

    struct Clouds
    {
        int64_t all;
    };

    struct Coord
    {
        double lon;
        double lat;
    };

    struct MainCurrentWeather
    {
        double temp;
        double feels_like;
        double temp_min;
        double temp_max;
        int64_t pressure;
        int64_t humidity;
    };

    struct Sys
    {
        std::string country;
        int64_t sunrise;
        int64_t sunset;
    };

    struct MainForecast
    {
        double temp;
        double feels_like;
        double temp_min;
        double temp_max;
        int64_t pressure;
        int64_t sea_level;
        int64_t grnd_level;
        int64_t humidity;
        double temp_kf;
    };

    struct Rain
    {
        double the_3_h;
    };

    enum class Pod : int
    {
        D,
        N
    };

    struct SysForecast
    {
        Pod pod;
    };

    enum class Description : int
    {
        BROKEN_CLOUDS,
        CLEAR_SKY,
        FEW_CLOUDS,
        LIGHT_RAIN,
        OVERCAST_CLOUDS,
        SCATTERED_CLOUDS
    };

    enum class MainEnum : int
    {
        CLEAR,
        CLOUDS,
        RAIN
    };

    struct Weather
    {
        int64_t id;
        std::string main;
        std::string description;
        std::string icon;
    };

    struct WindForecast
    {
        double speed;
        int64_t deg;
        double gust;
    };

    struct Wind
    {
        double speed;
        int64_t deg;
    };

    struct ForecastListItem
    {
        int64_t dt;
        MainForecast main;
        std::vector<Weather> weather;
        Clouds clouds;
        WindForecast wind;
        int64_t visibility;
        double pop;
        SysForecast sys;
        std::string dt_txt;
        std::optional<Rain> rain;
    };

    struct City
    {
        int64_t id;
        std::string name;
        Coord coord;
        std::string country;
        int64_t population;
        int64_t timezone;
        int64_t sunrise;
        int64_t sunset;
    };

    struct Forecast
    {
        std::string cod;
        int64_t message;
        int64_t cnt;
        std::vector<ForecastListItem> forecast;
        City city;
    };

    struct CurrentWeather
    {
        Coord coord;
        std::vector<Weather> weather;
        std::string base;
        MainCurrentWeather main;
        int64_t visibility;
        Wind wind;
        Clouds clouds;
        int64_t dt;
        Sys sys;
        int64_t timezone;
        int64_t id;
        std::string name;
        int64_t cod;
    };

    template <typename T>
    inline std::optional<T> get_stack_optional(const json &j, const char *property)
    {
        auto it = j.find(property);
        if (it != j.end() && !it->is_null())
        {
            return j.at(property).get<T>();
        }
        return std::optional<T>();
    }

    template <typename T>
    inline std::optional<T> get_stack_optional(const json &j, std::string property)
    {
        return get_stack_optional<T>(j, property.data());
    }

    inline void from_json(const json &j, Coord &x)
    {
        x.lat = j.at("lat").get<double>();
        x.lon = j.at("lon").get<double>();
    }

    inline void from_json(const json &j, City &x)
    {
        x.id = j.at("id").get<int64_t>();
        x.name = j.at("name").get<std::string>();
        x.coord = j.at("coord").get<Coord>();
        x.country = j.at("country").get<std::string>();
        x.population = j.at("population").get<int64_t>();
        x.timezone = j.at("timezone").get<int64_t>();
        x.sunrise = j.at("sunrise").get<int64_t>();
        x.sunset = j.at("sunset").get<int64_t>();
    }

    inline void from_json(const json &j, Clouds &x)
    {
        x.all = j.at("all").get<int64_t>();
    }

    inline void from_json(const json &j, MainForecast &x)
    {
        x.temp = j.at("temp").get<double>();
        x.feels_like = j.at("feels_like").get<double>();
        x.temp_min = j.at("temp_min").get<double>();
        x.temp_max = j.at("temp_max").get<double>();
        x.pressure = j.at("pressure").get<int64_t>();
        x.sea_level = j.at("sea_level").get<int64_t>();
        x.grnd_level = j.at("grnd_level").get<int64_t>();
        x.humidity = j.at("humidity").get<int64_t>();
        x.temp_kf = j.at("temp_kf").get<double>();
    }

    inline void from_json(const json &j, Rain &x)
    {
        x.the_3_h = j.at("3h").get<double>();
    }

    inline void from_json(const json &j, SysForecast &x)
    {
        x.pod = j.at("pod").get<Pod>();
    }

    inline void from_json(const json &j, WindForecast &x)
    {
        x.speed = j.at("speed").get<double>();
        x.deg = j.at("deg").get<int64_t>();
        x.gust = j.at("gust").get<double>();
    }

    inline void from_json(const json &j, Weather &x)
    {
        x.id = j.at("id").get<int64_t>();
        x.main = j.at("main").get<std::string>();
        x.description = j.at("description").get<std::string>();
        x.icon = j.at("icon").get<std::string>();
    }

    inline void from_json(const json &j, ForecastListItem &x)
    {
        x.dt = j.at("dt").get<int64_t>();
        x.main = j.at("main").get<MainForecast>();
        x.weather = j.at("weather").get<std::vector<Weather>>();
        x.clouds = j.at("clouds").get<Clouds>();
        x.wind = j.at("wind").get<WindForecast>();
        x.visibility = j.at("visibility").get<int64_t>();
        x.pop = j.at("pop").get<double>();
        x.sys = j.at("sys").get<SysForecast>();
        x.dt_txt = j.at("dt_txt").get<std::string>();
        x.rain = get_stack_optional<Rain>(j, "rain");
    }

    inline void from_json(const json &j, Forecast &x)
    {
        x.cod = j.at("cod").get<std::string>();
        x.message = j.at("message").get<int64_t>();
        x.cnt = j.at("cnt").get<int64_t>();
        x.forecast = j.at("list").get<std::vector<ForecastListItem>>();
        x.city = j.at("city").get<City>();
    }

    inline void from_json(const json &j, Pod &x)
    {
        if (j == "d")
            x = Pod::D;
        else if (j == "n")
            x = Pod::N;
        else
        {
            throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void from_json(const json &j, Description &x)
    {
        if (j == "broken clouds")
            x = Description::BROKEN_CLOUDS;
        else if (j == "clear sky")
            x = Description::CLEAR_SKY;
        else if (j == "few clouds")
            x = Description::FEW_CLOUDS;
        else if (j == "light rain")
            x = Description::LIGHT_RAIN;
        else if (j == "overcast clouds")
            x = Description::OVERCAST_CLOUDS;
        else if (j == "scattered clouds")
            x = Description::SCATTERED_CLOUDS;
        else
        {
            throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void from_json(const json &j, MainEnum &x)
    {
        if (j == "Clear")
            x = MainEnum::CLEAR;
        else if (j == "Clouds")
            x = MainEnum::CLOUDS;
        else if (j == "Rain")
            x = MainEnum::RAIN;
        else
        {
            throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void from_json(const json &j, MainCurrentWeather &x)
    {
        x.temp = j.at("temp").get<double>();
        x.feels_like = j.at("feels_like").get<double>();
        x.temp_min = j.at("temp_min").get<double>();
        x.temp_max = j.at("temp_max").get<double>();
        x.pressure = j.at("pressure").get<int64_t>();
        x.humidity = j.at("humidity").get<int64_t>();
    }

    inline void from_json(const json &j, Sys &x)
    {
        x.country = j.at("country").get<std::string>();
        x.sunrise = j.at("sunrise").get<int64_t>();
        x.sunset = j.at("sunset").get<int64_t>();
    }

    inline void from_json(const json &j, Wind &x)
    {
        x.speed = j.at("speed").get<double>();
        x.deg = j.at("deg").get<int64_t>();
    }

    inline void from_json(const json &j, CurrentWeather &x)
    {
        x.coord = j.at("coord").get<Coord>();
        x.weather = j.at("weather").get<std::vector<Weather>>();
        x.base = j.at("base").get<std::string>();
        x.main = j.at("main").get<MainCurrentWeather>();
        x.visibility = j.at("visibility").get<int64_t>();
        x.wind = j.at("wind").get<Wind>();
        x.clouds = j.at("clouds").get<Clouds>();
        x.dt = j.at("dt").get<int64_t>();
        x.sys = j.at("sys").get<Sys>();
        x.timezone = j.at("timezone").get<int64_t>();
        x.id = j.at("id").get<int64_t>();
        x.name = j.at("name").get<std::string>();
        x.cod = j.at("cod").get<int64_t>();
    }

    /**
     * @brief Get the Data object
     * 
     * @tparam DataStructClass 
     * @param strjson 
     * @return DataStructClass 
     */
    template <typename DataStructClass>
    DataStructClass GetData(const json &strjson)
    {

        if constexpr (std::is_same_v<DataStructClass, Forecast>)
        {
            Forecast x;
            from_json(strjson, x);
            return x;
        }
        else if constexpr (std::is_same_v<DataStructClass, CurrentWeather>)
        {
            CurrentWeather x;
            from_json(strjson, x);
            return x;
        }
        else if constexpr (std::is_same_v<DataStructClass, GeocodingResponse>)
        {
            auto n = strjson["name"];
            auto c = strjson["country"];
            auto p = Place{strjson["lat"], strjson["lon"]};
            auto g = GeocodingResponse{n, c, p};
            Logger::Print(Logger::Level::DEBUG, g.toString());
            return g;
        }
        else
        {
            static_assert(0);
        }
    };
};

#endif