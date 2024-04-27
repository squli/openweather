#include "api_handler.hpp"

static size_t VectorResponseCallback(char *dest, size_t size, size_t nmemb,
                                     std::vector<uint8_t> *userp)
{
    std::vector<char> data(dest, dest + size * nmemb);
    userp->insert(userp->end(), data.begin(), data.end());
    return size * nmemb;
}

static size_t ResponseCallback(char *dest, size_t size, size_t nmemb,
                               std::string *userp)
{
    userp->append(dest, size * nmemb);
    return size * nmemb;
}

bool ApiHandler::GetPlace(const std::string &city, Place &place)
{
    auto url = getUrl<OpenWeatherApiRequestTypes::kGeocoding, std::string>(city);
    auto response = makeSingleRequest(url);
    auto response_ = response.substr(1, response.size() - 2);
    Logger::Print(Logger::Level::DEBUG, "Geocoding response: " + response_);
    auto placeLocation = Json::GetData<GeocodingResponse>(Json::json::parse(response_));
    place = placeLocation.place;

    return true;
}

std::vector<std::vector<uint8_t>>
ApiHandler::MakeRequests(std::vector<std::string> url)
{
    size_t requestsCount = url.size();
    CURLM *curlm = NULL;
    int still_running = requestsCount;
    curlm = curl_multi_init();

    std::vector<CURL *> curls;
    for (size_t i = 0; i < requestsCount; i++)
    {
        CURL *curl = NULL;
        curl = curl_easy_init();
        curls.push_back(curl);
    }

    Logger::Print(Logger::Level::DEBUG, "Start multi-request");

    bool curlNotInit = false;
    for (CURL *url : curls)
    {
        if (url == NULL)
        {
            curlNotInit = true;
        }
    }

    std::vector<uint8_t> dflt;
    std::vector<std::vector<uint8_t>> response(requestsCount, dflt);

    if (!curlNotInit)
    {
        struct curl_slist *list = NULL;
        list = curl_slist_append(list, "User-Agent: Mozilla/5.0 (Macintosh; Intel "
                                       "Mac OS X x.y; rv:42.0) Gecko/20100101");

        for (size_t i = 0; i < requestsCount; i++)
        {
            curl_easy_setopt(curls[i], CURLOPT_URL, url[i].c_str());
            curl_easy_setopt(curls[i], CURLOPT_WRITEDATA, &(response[i]));
            curl_easy_setopt(curls[i], CURLOPT_WRITEFUNCTION,
                             &VectorResponseCallback);
            curl_easy_setopt(curls[i], CURLOPT_TIMEOUT, 3L);
            curl_easy_setopt(curls[i], CURLOPT_HTTPHEADER, list);
            curl_multi_add_handle(curlm, curls[i]);
        }
        // TODO: add response codes handler and check headers.
        while (still_running)
        {
            curl_multi_perform(curlm, &still_running);
        }
    }
    else
    {
        Logger::Print(Logger::Level::ERR, "curl return error");
    }

    for (CURL *url : curls)
    {
        curl_multi_remove_handle(curlm, url);
        curl_easy_cleanup(url);
    }
    curl_multi_cleanup(curlm);

    Logger::Print(Logger::Level::DEBUG, "Clean up multi-request, response.size=" +
                                          std::to_string(response.size()));
    return response;
}

std::pair<std::vector<std::string>, std::vector<std::string>>
ApiHandler::MakeVectorOfMapUrls(
    const WeatherMapSettings &weatherMapSettings) const
{
    std::vector<std::string> phUrls;
    std::vector<std::string> wUrls;
    WeatherMapSettings temp = weatherMapSettings;

    auto phUrl = getUrl<OpenWeatherApiRequestTypes::kMaps, WeatherMapSettings>(
        weatherMapSettings);
    auto wUrl =
        getUrl<OpenWeatherApiRequestTypes::kWeatherMaps, WeatherMapSettings>(
            weatherMapSettings);
    phUrls.push_back(phUrl);
    wUrls.push_back(wUrl);

    temp.y -= 1;
    phUrl = getUrl<OpenWeatherApiRequestTypes::kMaps, WeatherMapSettings>(temp);
    wUrl = getUrl<OpenWeatherApiRequestTypes::kWeatherMaps, WeatherMapSettings>(
        temp);
    phUrls.push_back(phUrl);
    wUrls.push_back(wUrl);

    temp.y += 2;
    phUrl = getUrl<OpenWeatherApiRequestTypes::kMaps, WeatherMapSettings>(temp);
    wUrl = getUrl<OpenWeatherApiRequestTypes::kWeatherMaps, WeatherMapSettings>(
        temp);
    phUrls.push_back(phUrl);
    wUrls.push_back(wUrl);

    temp.x -= 1;
    temp.y -= 1;
    phUrl = getUrl<OpenWeatherApiRequestTypes::kMaps, WeatherMapSettings>(temp);
    wUrl = getUrl<OpenWeatherApiRequestTypes::kWeatherMaps, WeatherMapSettings>(
        temp);
    phUrls.push_back(phUrl);
    wUrls.push_back(wUrl);

    temp.y -= 1;
    phUrl = getUrl<OpenWeatherApiRequestTypes::kMaps, WeatherMapSettings>(temp);
    wUrl = getUrl<OpenWeatherApiRequestTypes::kWeatherMaps, WeatherMapSettings>(
        temp);
    phUrls.push_back(phUrl);
    wUrls.push_back(wUrl);

    temp.y += 2;
    phUrl = getUrl<OpenWeatherApiRequestTypes::kMaps, WeatherMapSettings>(temp);
    wUrl = getUrl<OpenWeatherApiRequestTypes::kWeatherMaps, WeatherMapSettings>(
        temp);
    phUrls.push_back(phUrl);
    wUrls.push_back(wUrl);

    temp.x += 2;
    temp.y -= 1;
    phUrl = getUrl<OpenWeatherApiRequestTypes::kMaps, WeatherMapSettings>(temp);
    wUrl = getUrl<OpenWeatherApiRequestTypes::kWeatherMaps, WeatherMapSettings>(
        temp);
    phUrls.push_back(phUrl);
    wUrls.push_back(wUrl);

    temp.y -= 1;
    phUrl = getUrl<OpenWeatherApiRequestTypes::kMaps, WeatherMapSettings>(temp);
    wUrl = getUrl<OpenWeatherApiRequestTypes::kWeatherMaps, WeatherMapSettings>(
        temp);
    phUrls.push_back(phUrl);
    wUrls.push_back(wUrl);

    temp.y += 2;
    phUrl = getUrl<OpenWeatherApiRequestTypes::kMaps, WeatherMapSettings>(temp);
    wUrl = getUrl<OpenWeatherApiRequestTypes::kWeatherMaps, WeatherMapSettings>(
        temp);
    phUrls.push_back(phUrl);
    wUrls.push_back(wUrl);

    return std::pair<std::vector<std::string>, std::vector<std::string>>{wUrls,
                                                                         phUrls};
}

std::string ApiHandler::makeSingleRequest(const std::string &url)
{
    std::string response{};
    std::string responseHeaders{};
    CURL *curl = NULL;

    curl = curl_easy_init();
    if (curl == nullptr)
    {
        Logger::Print(Logger::Level::ERR, "Can't init curl");
        return response;
    }

    Logger::Print(Logger::Level::DEBUG, "Start request to " + url);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4L); // Timeout in seconds
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &ResponseCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeaders);

    long http_code = 0;
    CURLcode code = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200 || CURLE_OK != code)
    {
        curl_easy_cleanup(curl);
        Logger::Print(Logger::Level::ERR,
                      "Can't make request, code=" +
                          std::to_string(static_cast<int32_t>(code)));
        return response;
    }

    std::string asString(response.begin(), response.end());
    if (responseHeaders.find("Content-Type: application/json") !=
            std::string::npos)
    {
        Logger::Print(Logger::Level::DEBUG, "Got valid json");
    }

    curl_easy_cleanup(curl);
    Logger::Print(Logger::Level::DEBUG, "Clean up request to " + url);

    return response;
};
