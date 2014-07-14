/*
Monkey motion detector 
Written and developed: Alireza Saberi 
 
Dr. Amir Shmuel lab,
Montreal Neurological Institute,
McGill University,
Montreal, QC, Canada
Jan-Feb 2014

Copy right: Under permission from Dr. Amir Shmuel

*/

#include "stdafx.h"
#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include <conio.h>
#include <fstream>

using namespace cv;
using namespace std;

//This function is for clock
const std::string currentDateTime();

int main(int argc, char *argv[])
{
	ofstream Motion;
    Mat frame,SteupFrame;
    Mat back1, back2;
    Mat fore1, fore2;
	Point pt1,pt2;

    VideoCapture cap(0);
	cap >> SteupFrame;

	// Initialization and setup
	// ============================================================================
	cout << "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *" << endl;
	cout << "*	   Monkey Motion Detector                                          *" << endl;
	cout << "*     Copyright : Dr. Amir Shmuel lab  (amir.shmuel@mcgill.ca)             *" << endl;
	cout << "*	   Developed by: Alireza Saberi                     	           *" << endl;
	cout << "*	   Date: Jan-Feb 2014                                              *" << endl;
	cout << "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *" << endl;
	cout << "      Setting up and initialization ..." << endl;
	cout << endl;
	cout << "      Width of video is : "  << SteupFrame.cols << endl; 
	cout << "      Height if video is: " << SteupFrame.rows << endl;
	cout << endl;
	cv::namedWindow("Setup window");
	bool decision = true;
	char DES;
	int x,y,Width,Length;

	while (decision)
	{
		cap >> SteupFrame;
		cv::imshow("Setup window",SteupFrame);
		cv::waitKey(200);
		cout << "      Please enter your ROI specifications the following in order..." << endl;
		cout << endl;
		cout << "      Upper right corner X: " ;
		cin >> x ;
		cout << "      Upper right corner Y: ";
		cin >> y;
		cout << "      The length of square: ";
		cin >> Length;
		cout << "      The width of square: ";
		cin >> Width;
		cout << endl;
		cout << "      You entered (" << x << "," << y << ")" << " and L = " << Length << " W = " << Width << endl ;
		cout << endl;
		pt1.x = x;
		pt1.y = y;
		pt2.x = x + Length;
		pt2.y = y + Width;
		rectangle(SteupFrame, pt1, pt2, Scalar( 255, 0, 0 ), 3, 8, 0);
		cv::imshow("Setup window",SteupFrame);
		cv::waitKey(500);
		cout << "      Happy with determined ROI? <y/n> : ";
		cin >> DES;
		if (DES == 'y')
		{
			break;
		}
	}
	cout << endl;
	cout << "      No. of contour in ROI and out of ROI is displayed here" << endl;
	cout << "-------------------------------------------------------------------" << endl;
	cout << "NOT ROI \t ROI" << endl;
	destroyWindow("Setup window");
	// =============================================================================
	
	//BackgroundSubtractorMOG2 bg(int history,  float varThreshold, bool bShadowDetection=true);
    BackgroundSubtractorMOG2 bg1(3,  6, false);
	BackgroundSubtractorMOG2 bg2(3,  6, false);
	
    std::vector<std::vector<cv::Point>> contours1;
    std::vector<std::vector<cv::Point>> contours2;
    cv::namedWindow("Ali Monkey Motion Detector Window");
     for(;;)
    {
		// Motion detection algorithm is here 
		// ================== Whole frame ====================
		cap >> frame;
        bg1.operator ()(frame,fore1);
        bg1.getBackgroundImage(back1);
        cv::erode(fore1,fore1,cv::Mat());
        cv::dilate(fore1,fore1,cv::Mat());
        cv::findContours(fore1,contours1,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
        //cv::drawContours(frame,contours1,-1,cv::Scalar(0,0,255),2);
		int nAllContours = contours1.size();

		//diplsay something on the top of processed image for motion detection
		putText(frame,"Motion Detection",Point(10,25),2,0.5,Scalar(0,255,0),2);
		putText(frame,currentDateTime(),Point(10,50),2,0.5,Scalar(0,0,255),2);
		rectangle(frame, pt1, pt2, Scalar( 255, 0, 0 ), 3, 8, 0);		
		
		//if(cv::waitKey(100) == 53) 
		//{
		//putText(frame,"Trigger: YES ",Point(10,75),2,0.5,Scalar(0,0,255),2);
		//cv::imshow("Ali Gaussian Motion Detection Window",frame);

		//}
		//else
		//{
		//putText(frame,"Trigger: NO ",Point(10,75),2,0.5,Scalar(0,0,255),2);
		//cv::imshow("Ali Gaussian Motion Detection Window",frame);
		//}
		// ========================= Making ROI ============================
			cv::Rect roi;
			roi.x = pt1.x;
			roi.y = pt1.y;
			roi.width = pt2.x - pt1.x;
			roi.height = pt2.y - pt1.y;
			cv::Mat crop = frame(roi);
			bg2.operator ()(crop,fore2);
			bg2.getBackgroundImage(back2);
			cv::erode(fore2,fore2,cv::Mat());
			cv::dilate(fore2,fore2,cv::Mat());
			cv::findContours(fore2,contours2,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
			int nROIContours = contours2.size();
			int ROUout = nAllContours - nROIContours;

			cout << ROUout  << " \t\t " << nROIContours << endl;
            
			cv::drawContours(frame,contours1,-1,cv::Scalar(0,0,255),2);
			cv::waitKey(100);
			cv::imshow("Ali Monkey Motion Detector Window",frame);

		// CSV file part
		//ofstream Motion;
		Motion.open("MotionDetector.csv");
		Motion << nROIContours << endl;
		Motion << ROUout << endl;
		//Motion << std::to_string(2LL) <<endl;
		Motion.close();
		}
	    return 0;
}