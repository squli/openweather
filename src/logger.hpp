#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <iostream>
#include <array>
#include <mutex>

/**
 * @brief 
 * 
 */
struct Logger
{

    enum class Level : uint8_t
    {
        ERR = 0,
        INFO,  //< Default level with printing only necessary messages
        DEBUG, //< Extended logging to show more data for end-user
        kCount,
    };

    static inline const std::array<std::string, static_cast<size_t>(Logger::Level::kCount)> LevelNames{"ERROR", "INFO", "DBG"};
    
    static std::mutex mutex;
    
    static void Print(Logger::Level lvl, std::string_view msg)
    {
        const std::lock_guard<std::mutex> lock(Logger::mutex);
        if (lvl == Level::DEBUG && !verbose)
        {
            return;
        }
        auto &out = (lvl < Level::INFO) ? std::cerr : std::cout;
        out << "[" << LevelNames[static_cast<size_t>(lvl)] << "] " << msg << std::endl;
        out.flush();
    };

    static void EnableDebug(bool v)
    {
        verbose = v;
    }

    static bool inline verbose = false;
};

#endif