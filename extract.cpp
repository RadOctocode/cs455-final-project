#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char* argv[]){
	string input_image = argv[1];
	Mat orig_img = imread(input_image);
	Mat out_img = orig_img;
	pyrDown(orig_img, out_img);
	namedWindow("Original Out Image", WINDOW_AUTOSIZE);
	imshow("Original Out Image", out_img);
	Mat small;
	cvtColor(out_img, small, CV_BGR2GRAY);

	namedWindow("Original Image", WINDOW_AUTOSIZE);
	imshow("Original Image", orig_img);

	Mat grad;
	Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
	namedWindow("Grad Image", WINDOW_AUTOSIZE);
	imshow("Grad Image", grad);

	Mat bin_img;
	threshold(grad, bin_img, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
	namedWindow("Binary Image", WINDOW_AUTOSIZE);
	imshow("Binary Image", bin_img);

	Mat connected;
	morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
	morphologyEx(bin_img, connected, MORPH_CLOSE, morphKernel);
	namedWindow("Connected Image", WINDOW_AUTOSIZE);
	imshow("Connected Image", connected);

	Mat mask = Mat::zeros(bin_img.size(), CV_8UC1);
	vector<vector<Point>> contour;
	vector<Vec4i> hierarchy;
	findContours(connected, contour, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

	namedWindow("Out Image", WINDOW_AUTOSIZE);
	imshow("Out Image", out_img);
	
	int contour_index = 0;
	while(contour_index >= 0){
		Rect rect = boundingRect(contour[contour_index]);
		Mat maskROI(mask, rect);
		maskROI = Scalar(0,0,0);
		drawContours(mask, contour, contour_index, Scalar(255,255,255), CV_FILLED);
		double ratio = (double)countNonZero(maskROI)/(rect.width*rect.height);
		if(ratio > .50 && (rect.height > 10 && rect.width > 10)){
			rectangle(out_img, rect, Scalar(0,255,0), 2);
		}
		contour_index = hierarchy[contour_index][0];
	}
	imwrite("final.jpg", out_img);	
		
	waitKey();
	destroyAllWindows();
}
