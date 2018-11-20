#include <iostream>
#include <WebcamScanner.hpp>

int main(int argc, char** argv )
{

    std::string output_folder_name("scans");
    std::vector<cv::Mat> recorded_images;

    bool stage_success = true;
    WebcamScanner ws;

    if(argc <= 1)
    {
        std::cout << "Using webcam" << std::endl;

        cv::VideoCapture cap(0); // open the default camera
        if(!cap.isOpened())  // check if we succeeded
            return -1;

        ws.usage_stage_1();

        stage_success = ws.stage_1(cap, recorded_images);

        if(! stage_success)
        {
            return -1;
        }

        cv::destroyAllWindows();

    } else {
        std::cout << "Using existing folder of images" << std::endl;

        std::string input_foldername(argv[1]);
        std::cout << input_foldername << std::endl;



        ws.read_images(input_foldername, recorded_images);
    }
    

    std::cout << "Using " << recorded_images.size() << " images for Stage 2" << std::endl;
    
    ws.usage_stage_2();

    // define Din A4
    float height_width_ratio = 297.0 / 210.0;
    // define resolution
    int width = 800;

    std::vector<cv::Mat> transformed_images;
    stage_success = ws.stage_2(recorded_images, height_width_ratio, width, transformed_images);

    if(!stage_success)
    {
        return -1;
    }

    ws.save_images(transformed_images, output_folder_name);


    return 0;
}