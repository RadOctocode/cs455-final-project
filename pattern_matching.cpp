#include <opencv2/opencv.hpp>
#include <opencv2/plot.hpp>
#include <cmath>
#include <vector>
#include <queue>
#include <iostream>
#include <vector>
#include "morphological_ops.cpp"

void pattern_matching_tests();
Mat union_binary_images(Mat bin_img1, Mat bin_img2);
bool isZero(Mat bin_img);
Mat skeleton(Mat binary_image, Kernel k);
// subtracts bin_img2 from bin_img1
Mat subtract(Mat bin_img1, Mat bin_img2);

int main() {
	pattern_matching_tests();
	return 0;
}

Mat subtract(Mat bin_img1, Mat bin_img2) {
	if(bin_img1.rows != bin_img2.rows || bin_img1.cols != bin_img2.cols) 
		std::invalid_argument("bin_img1 & bin_img2 sizes must be the same!");
	Mat ret_val = bin_img1.clone();
	for(int i=0; i < bin_img1.rows; i++) {
		for(int j=0; j < bin_img1.cols; j++) {
			int diff = (int)bin_img1.at<uchar>(i,j) - (int)bin_img2.at<uchar>(i,j);
			if(diff < 0) diff = 0;
			ret_val.at<uchar>(i,j) = diff;
		}
	}
	return ret_val;
}

Mat union_binary_images(Mat bin_img1, Mat bin_img2) {
	if(bin_img1.rows != bin_img2.rows || bin_img1.cols != bin_img2.cols) 
		std::invalid_argument("bin_img1 & bin_img2 sizes must be the same!");
	Mat ret_val = bin_img1.clone();
	for(int i=0; i < bin_img1.rows; i++) {
		for(int j=0; j < bin_img1.cols; j++) {
			if((int)bin_img1.at<uchar>(i,j) == 255 || (int)bin_img2.at<uchar>(i,j) == 255) 
				ret_val.at<uchar>(i,j) = 255;
			else ret_val.at<uchar>(i,j) = 0;
		}
	}
	return ret_val;
}
bool isZero(Mat bin_img) {
	for(int i=0; i < bin_img.rows; i++) {
		for(int j=0; j < bin_img.cols; j++) {
			if((int)bin_img.at<uchar>(i,j) != 0) return false;
		}
	}
	return true;
}
void pattern_matching_tests() {
	Mat image = imread("glyphs/0.png", CV_LOAD_IMAGE_COLOR);
	Mat gray_image;
 	cvtColor( image, gray_image, COLOR_BGR2GRAY );
 	namedWindow( "Gray Scale", WINDOW_AUTOSIZE);
 	imshow( "Gray Scale", gray_image);

 	waitKey(0);

 	Mat bin_img = binary(gray_image, 100);
	namedWindow( "Binary Image", WINDOW_AUTOSIZE);
 	imshow( "Binary Image", bin_img);

 	waitKey(0);
	
	Mat neg = negative(bin_img); 
	namedWindow( "Negative Image", WINDOW_AUTOSIZE);
 	imshow( "Negative Image", neg);

 	waitKey(0);

 	Mat image2 = imread("glyphs/B.png", CV_LOAD_IMAGE_COLOR);
	Mat gray_image2;
 	cvtColor( image2, gray_image2, COLOR_BGR2GRAY );
 	namedWindow( "Gray Scale 2", WINDOW_AUTOSIZE);
 	imshow( "Gray Scale 2", gray_image2);

 	waitKey(0);

 	Mat bin_img2 = binary(gray_image2, 100);
	namedWindow( "Binary Image 2", WINDOW_AUTOSIZE);
 	imshow( "Binary Image 2", bin_img2);

 	waitKey(0);

 	Mat unioned = negative(union_binary_images(negative(bin_img), negative(bin_img2)));

 	namedWindow( "Union A, B", WINDOW_AUTOSIZE);
 	imshow( "Union A, B", unioned);

 	waitKey(0);

 	Mat subtracted = negative(subtract(negative(bin_img), negative(bin_img2)));
	
	namedWindow( "A - B", WINDOW_AUTOSIZE);
 	imshow( "A - B", subtracted);

 	waitKey(0); 	
 	//basic kernel
 	Kernel erosion_kernel1(1, 1, 3, 3);
 	int erosion_data_2D_array[3][3] = {{0,1,0}, {1,1,1}, {0,1,0}};
 	for(int i=0; i< 3; i++) {
 		for(int j=0; j< 3; j++) {
 			erosion_kernel1.data[i][j] = erosion_data_2D_array[i][j];
 		}
 	}	
 	Mat skeletoned = negative(skeleton(neg, erosion_kernel1));
 	// Mat skeletoned = skeleton(bin_img, erosion_kernel1);
 	namedWindow( "Skeleton", WINDOW_AUTOSIZE);
 	imshow( "Skeleton", skeletoned);

 	waitKey(0);
}

Mat skeleton(Mat binary_image, Kernel k) {

	// Mat eroded_n = erosion_iter(binary_image, k, 3);
	// Mat eroded_opened_n = opening(eroded_n, k);
	// return eroded_opened_n;
	int n = 0;
	Mat eroded_n;
	Mat eroded_opened_n;
	Mat eroded_n_minus_eroded_opened_n;
	Mat skeleton = binary_image.clone();
	for(int i=0; i < skeleton.rows; i++) {
		for(int j=0; j < skeleton.cols; j++) skeleton.at<uchar>(i,j) = 0;
	}
	do {
		n += 1;
		eroded_n = erosion_iter(binary_image, k, n);
		eroded_opened_n = opening(eroded_n, k);
		eroded_n_minus_eroded_opened_n = subtract(eroded_n, eroded_opened_n);
		skeleton = union_binary_images(skeleton, eroded_n_minus_eroded_opened_n);
	} while(!isZero(eroded_opened_n));
	return skeleton;
}





