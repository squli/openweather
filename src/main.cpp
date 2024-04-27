#include "weather_picture_generator.hpp"

using namespace std::chrono_literals;

// To get rid of the logger.cpp
std::mutex Logger::mutex;

int main(int argc, char *argv[])
{
    if (argc != 2 && argc != 3)
    {
        Logger::Print(Logger::Level::ERR, "Wrong amount of arguments, argc=" + std::to_string(argc));
        return 1;
    }

    std::vector<std::string> arguments(argv + 1, argv + argc);
    if (argc == 2 && (arguments.at(0).find("-h") != std::string::npos ||
                      arguments.at(0).find("help") != std::string::npos))
    {
        Logger::Print(Logger::Level::INFO, "First argument - city name, second argument is optional -v to verbose");
        return 2;
    }

    Logger::Print(Logger::Level::INFO, "City to make map: " + arguments.at(0));
    if (argc == 3 && arguments.at(1).find("-v") != std::string::npos)
    {
        Logger::EnableDebug(true);
    }

    if (const char *env_p = std::getenv("OPEN_WEATHER_API_KEY"))
    {
        const std::string_view apiKey{env_p};

        using ErrCode = WeatherPictureGenerator::WeatherPictureGeneratorErrors; 

        curl_global_init(CURL_GLOBAL_DEFAULT);
        WeatherPictureGenerator weatherPictureGenerator(apiKey, arguments.at(0));
        ErrCode error = weatherPictureGenerator.generatePicture();
        if (error != ErrCode::OK)
        {
            Logger::Print(Logger::Level::ERR, "Cant make a picture, code=" + static_cast<int>(error));
        }
        curl_global_cleanup();
        return static_cast<int>(error);
    }
    else
    {
        Logger::Print(Logger::Level::ERR, "Can't get OPEN_WEATHER_API_KEY env variable");
        return 2;
    }
}