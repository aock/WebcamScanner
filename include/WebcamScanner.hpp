#pragma once

// c++ includes
#include <iostream>
#include <string> 
#include <sstream>

// opencv includes
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>


namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}




class WebcamScanner {
public:
    WebcamScanner();
    void usage_stage_1();
    bool stage_1(cv::VideoCapture& cap, std::vector<cv::Mat>& output_images);
    void usage_stage_2();
    bool stage_2(std::vector<cv::Mat>& input_images,
        float target_height_width_ratio,
        int target_width,
        std::vector<cv::Mat>& output_images);

    void save_images(const std::vector<cv::Mat>& images, std::string folder);
    void read_images(std::string folder, std::vector<cv::Mat>& images);

private:
    
    int findNearestCorner(int x, int y);

    inline float distToCorner(const int& x, const int& y, const int& corner_id)
    {
        return std::sqrt( std::pow(x-corners[corner_id].x, 2) + std::pow(y-corners[corner_id].y, 2) );
    }

    void onMouse(int event, int x, int y);
    static void onMouse(int event, int x, int y, int, void* userdata);

    // mouse event params
    const float max_dist_corner = 10.0;
    bool dragged = false;
    int nearest_corner = 0;

    std::vector<cv::Point> corners;

    cv::Point point1, point2; 

};

