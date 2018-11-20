#include <WebcamScanner.hpp>

WebcamScanner::WebcamScanner() {
    corners.push_back(cv::Point(20,20));
    corners.push_back(cv::Point(100,20));
    corners.push_back(cv::Point(100,100));
    corners.push_back(cv::Point(20,100));
}

void WebcamScanner::usage_stage_1()
{
    std::cout << "STAGE 1 (recording):" << std::endl;
    std::cout << "\tr:\trecord image" << std::endl;
    std::cout << "\td:\tdelete last image" << std::endl;
    std::cout << "\tf:\tfinish recording and go to editing stage" << std::endl;
    std::cout << "\tESC:\tescape program" << std::endl;
}

bool WebcamScanner::stage_1(cv::VideoCapture& cap, std::vector<cv::Mat>& output_images) 
{
    cv::namedWindow("WebcamScanner", CV_WINDOW_AUTOSIZE);
    cv::Point text_pos(50,50);

    while(1)
    {
        cv::Mat frame, view_image;
        cap >> frame; // get a new frame from camera
        
        frame.copyTo(view_image);

        

        cv::putText(view_image, patch::to_string(output_images.size()), text_pos, 1, 2.0, cv::Scalar(0.0,0.0,0.0), 3);

        cv::imshow("WebcamScanner", view_image);
        int key = cv::waitKey(30);
        char key_char = (char)key;

        if(key >= 0)
        {
            if( key_char == 'f')
            {
                // finish
                return true;
            } else if( key_char == 'r')
            {
                // record image
                output_images.push_back(frame);
            } else if ( key_char == 'd')
            {
                // delete last image
                output_images.pop_back();
            } else if( key == 27)
            {
                // ESC
                return false;
            }
            
        }
        
    }

} 

void WebcamScanner::usage_stage_2()
{
    std::cout << "STAGE 2 (transforming):" << std::endl;
    std::cout << "select recangle on screen of the document" << std::endl;
    std::cout << "\tn:\tnext image" << std::endl;
    std::cout << "\tESC:\tescape program" << std::endl;
}



bool WebcamScanner::stage_2(std::vector<cv::Mat>& input_images,
    float target_height_width_ratio,
    int target_width,
    std::vector<cv::Mat>& output_images
)
{
    cv::namedWindow("Recorded Image", CV_WINDOW_NORMAL);
    cv::namedWindow("Transformed Image", CV_WINDOW_NORMAL);

    cv::setMouseCallback("Recorded Image", WebcamScanner::onMouse, this);

    cv::Point text_pos(50,50);

    cv::Size target_image_size(target_width, static_cast<int>(target_height_width_ratio * target_width) );

    std::cout << "generating image of size: "
              << target_image_size.width << " x "
              << target_image_size.height  << std::endl;

    std::vector<cv::Point2f> source_corners(4);

    std::vector<cv::Point2f> target_corners;
    target_corners.push_back(cv::Point2f(0,0));
    target_corners.push_back(cv::Point2f(target_image_size.width-1, 0));
    target_corners.push_back(cv::Point2f(target_image_size.width-1, target_image_size.height-1));
    target_corners.push_back(cv::Point2f(0, target_image_size.height-1));

    for(cv::Mat img : input_images)
    {
        
        while(1) {

            cv::Mat img_view;
            img.copyTo(img_view);

            

            std::vector<std::vector<cv::Point> > polylines;
            polylines.push_back(corners);
            cv::polylines(img_view, polylines, true, cv::Scalar(0.0, 0.0, 255.0), 2);
            cv::putText(img_view, patch::to_string(output_images.size()), text_pos, 1, 2.0, cv::Scalar(0.0,0.0,0.0), 3);

            cv::imshow("Recorded Image", img_view);

            cv::Mat transformed_image = cv::Mat::zeros(target_image_size, img.type());

            // img.copyTo(transformed_image);

            for(int i=0; i<corners.size(); i++)
            {
                source_corners[i].x = corners[i].x;
                source_corners[i].y = corners[i].y;
            }

            cv::Mat T;
            
            T = cv::getPerspectiveTransform(source_corners, target_corners);
            cv::warpPerspective(img, transformed_image, T, target_image_size);


            cv::imshow("Transformed Image", transformed_image);

            int key = cv::waitKey(30);
            char key_char = (char)key;

            if(key >= 0)
            {
                if( key_char == 'n')
                {
                    // finish
                    output_images.push_back(transformed_image);
                    break;
                } else if( key == 27)
                {
                    // ESC
                    return false;
                }
                
            }
        }
    }

    return true;
}

void WebcamScanner::save_images(const std::vector<cv::Mat>& images, std::string folder)
{
    const char* path = folder.c_str();
    boost::filesystem::path dir(path);
    if(boost::filesystem::create_directory(dir))
    {
        std::cout << "Directory Created: "<< folder << std::endl;
    }

    for(int i=0; i<images.size(); i++)
    {
        std::string curr_img_str = patch::to_string(i);
        std::string filename = folder + std::string("/converted") + curr_img_str + std::string(".png");
        cv::imwrite(filename, images[i]);
    }
}

void WebcamScanner::read_images(std::string folder, std::vector<cv::Mat>& images)
{
    std::vector<cv::String> fn;

    // png
    cv::glob(folder+"/*", fn, false);

    size_t count = fn.size(); //number of png files in images folder
    for (size_t i=0; i<count; i++)
    {
        images.push_back(cv::imread(fn[i]));
    }
}


// PRIVATE

int WebcamScanner::findNearestCorner(int x, int y)
{
    float best_dist = std::numeric_limits<float>::max();
    int best_index = 0;
    for(int i=0; i<corners.size(); i++)
    {
        float current_dist = std::pow(x - corners[i].x, 2.0) + std::pow(y - corners[i].y, 2.0);
        if(current_dist < best_dist)
        {
            best_dist = current_dist;
            best_index = i;
        }
    }

    return best_index;
}

void WebcamScanner::onMouse(int event, int x, int y)
{
    if( event == CV_EVENT_LBUTTONDOWN )
    {
        int n_corner_id = findNearestCorner(x,y);
        float dist = distToCorner(x, y, n_corner_id);

        if(dist < max_dist_corner)
        {
            nearest_corner = n_corner_id;
            dragged = true;
        } else {
            dragged = false;
        }
        
    } else if(event == CV_EVENT_LBUTTONUP)
    {
        dragged = false;
    }

    if(event == CV_EVENT_MOUSEMOVE && dragged)
    {
        corners[nearest_corner].x = x;
        corners[nearest_corner].y = y;
    }

}

// Implement it to call the member function
void WebcamScanner::onMouse(int event, int x, int y, int, void* userdata)
{
    // Check for null pointer in userdata and handle the error
    
    WebcamScanner* settings = reinterpret_cast<WebcamScanner*>(userdata);
    settings->onMouse(event, x, y);
}

