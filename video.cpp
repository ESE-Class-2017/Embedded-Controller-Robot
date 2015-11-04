#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>
#include "video.h"
#include <unistd.h>

using namespace std;
char key;
int video_cap()
{
    int i;
    int j;
    int a;
    int b;
    int c;
    double Area;

    double thresh = 92;
    double maxValue = 255;

    //cv::Mat image;
    cv::Mat gray;
    cv::Mat BW;
    cv::Mat frame;
    cv::Mat shot1;
    cv::Mat canny_out;
    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;

    // create window so that a new window is not created for each frame
    cv::namedWindow("Camera_Output", 1);
    cv::namedWindow("Image_Output", 1);

    //Open camera for capturing
    cv::VideoCapture capture(CV_CAP_ANY);


    // Create infinte loop for live streaming
    while(1)
    {

        capture >> frame;
        cv::imshow("Camera_Output", frame);            // Show image frames on created window
        key = cvWaitKey(10);                            // Capture Keyboard stroke

        if(image_quit == true)
        {
            //Ends the loop
            break;
        }

        else if(image_capture == true)
        {
            //Capture image
            capture >> shot1;
            cv::imshow("Image_Output", shot1);
            cv::imwrite("image.bmp", shot1);

            //Convert to grayscale
            cvtColor(shot1, gray, CV_BGR2GRAY);
            cv::imwrite("BW.bmp", gray);

            //Set grayscale thresholds
            threshold(gray, BW, thresh, maxValue, 1);

            //Blur the image
            GaussianBlur(BW, BW, cv::Size(5,5), 2, 2);


            //Used for more accuracy but caused bug in code
            //Detect edges with Canny
            //Canny(BW, canny_out, 100, 200, 3);

            //Find contours
            findContours( BW, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

            //Draw contours
            cv::Mat drawing = cv::Mat::zeros( BW.size(), CV_8UC3 );
            for( i = 0; i< contours.size(); i++ )
            {
                cv::Scalar color = cv::Scalar( 255, 255, 255 );
                drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
            }

            vector<vector<cv::Point> > contours_poly( contours.size() );


            for(i = 0; i < contours.size(); i++)
            {
                // Gets the points for the contours
                approxPolyDP(contours[i], contours_poly[i], cv::arcLength(cv::Mat(contours[i]), true) *0.05, true);

            }

            //Draw points and lines for shapes
            for(i = 0; i < contours_poly.size(); i++)
            {
                for(j = 0; j < contours_poly[i].size(); j++)
                {
                    cv::line(drawing, contours_poly[i][j], contours_poly[i][(j+1)%3], cv::Scalar(0, 255, 0), 2, 8);
                    cv::circle(drawing, contours_poly[i][j], 5, cv::Scalar (255, 0 , 0), 1, 8, 0);
                    //cout << contours_poly[i][j] << endl;

                }

                Area = contourArea(contours_poly[i], false);
                //cout << "Area: " << Area << endl;

                //shows what shapes were found
                if(contours_poly[i].size() == 3 && Area > 15000)
                {
                    a = fabs(contours_poly[i][0].x - contours_poly[i][1].x);
                    b = fabs(contours_poly[i][1].x - contours_poly[i][2].x);
                    c = fabs(contours_poly[i][2].x - contours_poly[i][0].x);

                        if(a < b && a < c)
                        {
                            if((contours_poly[i][0].x - contours_poly[i][2].x) > 0)
                                cout << "A left triangle" << endl;
                            else
                                cout << "A right triangle" << endl;
                        }
                        else if(b < a && b < c)
                        {
                            if((contours_poly[i][1].x - contours_poly[i][0].x) > 0)
                                cout << "A left triangle" << endl;
                            else
                                cout << "A right triangle" << endl;
                        }
                        else
                         {
                            if((contours_poly[i][0].x - contours_poly[i][1].x) > 0)
                                cout << "A left triangle" << endl;
                            else
                                cout << "A right triangle" << endl;
                         }

                }
                else if(contours_poly[i].size() == 4 && Area > 15000)
                    cout << "A rectangle was detected" << endl;
            }

            // Save the contours image
            cv::imwrite("drawing.bmp", drawing);

            // reset the image capture
            image_capture = false;


        }

    }


    cv::destroyWindow("Camera_Output");   //Destroy Window
    cv::destroyWindow("Image_Output");
    return 0;
}

