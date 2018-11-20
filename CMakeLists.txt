cmake_minimum_required(VERSION 2.8)
project( DisplayImage )

set (CMAKE_CXX_STANDARD 11)
find_package(OpenCV 3 REQUIRED HINTS /usr/local)
message(STATUS "Found OpenCV version ${OpenCV_VERSION}")
message(STATUS "Found OpenCV in ${OpenCV_INSTALL_PATH}")

find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(include)
add_executable( WebcamScanner src/main.cpp src/WebcamScanner.cpp )
target_link_libraries( WebcamScanner
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
)