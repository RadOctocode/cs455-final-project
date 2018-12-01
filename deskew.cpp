#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <algorithm>    
#include <iostream>
#include <math.h>
#include <stdlib.h>
using namespace cv;
using namespace std;

Mat thres_image(Mat image){
     Mat image_ret=image.clone();
     long average_shade=0;
     for(int i=0;i<image.){
     	for(int j=0;){
	
	
	}
     
     }    
     


}

Mat compute_skew(Mat image1,int lowest_thres){
	Mat image=image1.clone();
	threshold(image1,image,lowest_thres,255,THRESH_BINARY);
	bitwise_not(image,image);
	
	vector<Point>pts;
	Mat_<uchar>::iterator it = image.begin<uchar>();
	Mat_<uchar>::iterator end = image.end<uchar>();

	for(;it!=end;++it){
		if(*it){
			pts.push_back(it.pos());
		}
	}

	RotatedRect box=minAreaRect(Mat(pts));

	double ang=box.angle;
	if(ang < -45.0){
		ang +=90;
	}
	Point2f vrts[4];
	box.points(vrts);
	

	cout<<"----------------------"<<ang<<"-------------------"<<"\n";
	//imshow("res",image);
	Mat rot_im=getRotationMatrix2D(box.center,ang,1);
	imshow("rot",rot_im);
	Mat rotated;
	warpAffine(image,rotated,rot_im,image.size(),INTER_CUBIC);
	return rotated;
}

/*int main( int argc, char** argv ){

    Mat odd,even,thresEven,thresOdd;
    //char exit;
    even = imread("test2.jpg", 0);   // Read the file
    Mat test=even.clone();
    imshow("original",even);
    imshow("rotated",compute_skew(even,75));
    //imshow("My N1",N1(even));
    //imshow("My N2",N2(even));
    //imshow("My N3",N3(even,traindata));
    //imshow("My N4",N4(even));
    //imshow("My K1",K1(even));
    //imshow("My T1",fullycorrect);
  //  cout<<"-------------------------error rate-------------------------\n"<<calc_error(fullycorrect,N1(even))*100<<"\n"<<"-----------------------------------------";

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}*/


/*
 *K1 - k means
 *M1 - class thresholding see sheet
 *N1-N4 - nearest neighbor
 *T1 - manual grouping of 4 by 4
 *E - error btwn N1 to T1
 *
 *
 *row=y
 *col=x
 *
 * */

