

#include <cv.h>
#include <highgui.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace cv;
using namespace std;
cv::Mat src; cv::Mat src_gray;
int thresh = 100;
int max_thresh = 255;
void thresh_callback(int, void* );
IplImage* GetThresholdedImage(IplImage* imgHSV){
       IplImage* imgThresh=cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
       cvInRangeS(imgHSV, cvScalar(0,100,110), cvScalar(20,256,256), imgThresh);
       return imgThresh;
}

int main(){
      CvCapture* capture =0;

      capture = cvCaptureFromCAM(0);
      if(!capture){
printf("Capture failure\n");
return -1;
      }

    IplImage* frame= 0;
      cvNamedWindow("Video");
      cvNamedWindow("Ball");



      while(true){

            frame = cvQueryFrame(capture);
            if(!frame) break;

            frame=cvCloneImage(frame);
            cvSmooth(frame, frame, CV_GAUSSIAN,3,3); //smooth the original image using Gaussian kernel

            IplImage* imgHSV = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
            cvCvtColor(frame, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
            IplImage* imgThresh = GetThresholdedImage(imgHSV);
            IplImage* imgThresh2;
            cvSmooth(imgThresh, imgThresh, CV_GAUSSIAN,3,3); //smooth the binary image using Gaussian kernel


            cvSaveImage("out1.jpg", imgThresh);
            src = imread( "out1.jpg", 1 );

  /// Convert image to gr and blur it
  cvtColor( src, src_gray, CV_BGR2GRAY );
  blur( src_gray, src_gray, cv::Size(3,3) );

  /// Create Window
  //char* source_window = "Source";
  //namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  //imshow( source_window, src );

  createTrackbar( " Threshold:", "Source", &thresh, max_thresh, thresh_callback );
  thresh_callback( 0, 0 );
    imgThresh2 = cvLoadImage("out2.jpg");
            //Clean up used images
            cvShowImage("Ball", imgThresh2);
            cvShowImage("Video", frame);
            cvReleaseImage(&imgHSV);
            cvReleaseImage(&imgThresh);
            cvReleaseImage(&frame);
            cvReleaseImage(&imgThresh2);

            //Wait 50mS
            int c = cvWaitKey(10);
            //If 'ESC' is pressed, break the loop
            if((char)c==27 ) break;
      }

      cvDestroyAllWindows() ;
      cvReleaseCapture(&capture);

      return 0;
}

void thresh_callback(int, void* )
{
  cv::Mat threshold_output;
  cv::vector<cv::vector<cv::Point> > contours;
  cv::vector<cv::Vec4i> hierarchy;

  /// Detect edges using Threshold
  threshold( src_gray, threshold_output, thresh, 255, CV_THRESH_BINARY );
  /// Find contours
  findContours( threshold_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Approximate contours to polygons + get bounding rects and circles
  cv::vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );
  vector<Point2f>center( contours.size() );
  vector<float>radius( contours.size() );

  for( int i = 0; i < contours.size(); i++ )
     { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
       boundRect[i] = boundingRect( Mat(contours_poly[i]) );
       minEnclosingCircle( contours_poly[i], center[i], radius[i] );
     }

int area,maxarea=0;
  /// Draw polygonal contour + bonding rects + circles
  Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( 255, 0, 0 );
       drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
       rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
      // area =
       //maxarea
       //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
     }
        imwrite("out2.jpg", drawing);
  /// Show in a window
 // namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  //imshow( "Contours", drawing );
}
