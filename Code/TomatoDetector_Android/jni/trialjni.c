#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>

using namespace std;
using namespace cv;

extern "C" {
    cv::Mat src; cv::Mat src_gray;
int thresh = 100;
int max_thresh = 255;
int result[3]= {0,0,0};

void thresh_callback(int, void* );
    IplImage* GetThresholdedImage(IplImage* imgHSV){
       IplImage* imgThresh=cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
       cvInRangeS(imgHSV, cvScalar(0,100,110), cvScalar(20,256,256), imgThresh);
       return imgThresh;
}

JNIEXPORT jintArray JNICALL Java_org_opencv_samples_tutorial3_Sample3View_FindFeatures(JNIEnv* env, jobject, jint width, jint height, jbyteArray yuv, jintArray bgra)
{
   // jbyte* _yuv  = env->GetByteArrayElements(yuv, 0);
    //jint*  _bgra = env->GetIntArrayElements(bgra, 0);
//jintArray result = env->NewIntArray(6);
    //Mat myuv(height + height/2, width, CV_8UC1, (unsigned char *)_yuv);
    //Mat mbgra(height, width, CV_8UC4, (unsigned char *)_bgra);
    IplImage* frame = getIplImageFromIntArray(env, yuv, width, height);
    frame=cvCloneImage(frame);
    cvSmooth(frame, frame, CV_GAUSSIAN,3,3); //smooth the original image using Gaussian kernel
    IplImage* imgHSV = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
    cvCvtColor(frame, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
    IplImage* imgThresh = GetThresholdedImage(imgHSV);
    IplImage* imgThresh2;
    cvSmooth(imgThresh, imgThresh, CV_GAUSSIAN,3,3);
    cvSaveImage("out1.jpg", imgThresh);
    src = imread( "out1.jpg", 1 );
    cvtColor( src, src_gray, CV_BGR2GRAY );
  blur( src_gray, src_gray, cv::Size(3,3) );

  /// Create Window
  //char* source_window = "Source";
  //namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  //imshow( source_window, src );

  createTrackbar( " Threshold:", "Source", &thresh, max_thresh, thresh_callback );
  thresh_callback( 0, 0 );
    //Please make attention about BGRA byte order
    //ARGB stored in java as int array becomes BGRA at native level
    //cvtColor(myuv, mbgra, CV_YUV420sp2BGR, 4);

    //vector<KeyPoint> v;

    //FastFeatureDetector detector(50);
    //detector.detect(mgray, v);
    //for( size_t i = 0; i < v.size(); i++ )
     //   circle(mbgra, Point(v[i].pt.x, v[i].pt.y), 10, Scalar(0,0,255,255));

    env->ReleaseIntArrayElements(bgra, _bgra, 0);
    env->ReleaseByteArrayElements(yuv, _yuv, 0);
    return result;
}
IplImage* getIplImageFromIntArray(JNIEnv* env, jintArray array_data,
		jint width, jint height) {
	int *pixels = env->GetIntArrayElements(array_data, 0);
	if (pixels == 0) {
		LOGE("Error getting int array of pixels.");
		return 0;
	}
	IplImage *image = loadPixels(pixels, width, height);
	env->ReleaseIntArrayElements(array_data, pixels, 0);
	if (image == 0) {
		LOGE("Error loading pixel array.");
		return 0;
	}
	return image;
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

int area =0,maxarea=0, max_rect, imgcenter_x, imgcenter_y, center_x, center_y ;
  /// Draw polygonal contour + bonding rects + circles
  Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( 255, 0, 0 );
       drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
       rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
       area = abs((boundRect[i].tl()).y - (boundRect[i].br()).y)* abs((boundRect[i].tl()).x - (boundRect[i].br()).x);
        if(area>maxarea)
        { max_rect= i;
        maxarea = area;
        } result[0]=1;
        }
   result[1]= abs((boundRect[i].tl()).x - (boundRect[i].br()).x)/2;
   result[2]= abs((boundRect[i].tl()).y - (boundRect[i].br()).y)/2;
       //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );

       // imwrite("out2.jpg", drawing);
  /// Show in a window
 // namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  //imshow( "Contours", drawing );
}

}

