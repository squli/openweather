CXXFLAGS = -std=c++17 -fpermissive -O3 -Werror -Wall

HEADERS = src/json.hpp src/logger.hpp src/api_handler.hpp src/weather_handler.hpp src/maps_handler.hpp src/weather_picture_generator.hpp
SOURCES = src/main.cpp src/api_handler.cpp src/maps_handler.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# libcurl
INCLUDE = -Ilibs/curl-8.7.1_7-win64-mingw/include -Ijson/include -Iopencv/build/include -Isrc

LDFLAGS = -Llibs/curl-8.7.1_7-win64-mingw/lib -Llibs
LIBS = -lcurl -lopencv_core490 -lopencv_highgui490 -lopencv_imgcodecs490 -lopencv_imgproc490

default: openweather

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

openweather: $(OBJECTS)
	g++ $(CXXFLAGS) $(INCLUDE) $(OBJECTS) -o build/openweather $(LDFLAGS) $(LIBS)

clean:
	-del /q src/openweather.o
	-del /q src/api_handler.o
	-del /q src/maps_handler.o
	-del /q build/openweather.exe