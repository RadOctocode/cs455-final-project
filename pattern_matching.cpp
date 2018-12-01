#include <opencv2/opencv.hpp>
#include <opencv2/plot.hpp>
#include <cmath>
#include <vector>
#include <queue>
#include <iostream>
#include <vector>
#include "morphological_ops.cpp"

void skeleton_tests();
void preprocessing_tests();
Mat union_binary_images(Mat bin_img1, Mat bin_img2);
bool isZero(Mat bin_img);
Mat skeleton(Mat binary_image, Kernel k);
// subtracts bin_img2 from bin_img1
Mat subtract(Mat bin_img1, Mat bin_img2);


Mat remove_borders(Mat binary_image);
//scale img1  to img2 size
Mat scale_image(Mat bin_img1, Mat bin_img2, String axis);

//percentage value  0 -> 1
//percentage image 1 overlaps image 2
float percentage_overlap(Mat bin_img1, Mat bin_img2);


//remove borders off a binary image
//scale either along "x" or "y"
//percentage overlap for 2 binary images

int main() {
	// skeleton_tests();
	preprocessing_tests();
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
void skeleton_tests() {
	Mat image = imread("glyphs/_m.png", CV_LOAD_IMAGE_COLOR);
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
Mat scale_image(Mat bin_img1, Mat bin_img2, char axis) {
	if(axis != 'x' && axis != 'y') throw std::invalid_argument("axis must be either x or y");
	Mat bin_img1_copy = bin_img1.clone();
	float sz = 0;
	if(axis == 'x') sz = bin_img2.cols /(float)bin_img1.cols; 
	else sz = bin_img2.rows /(float)bin_img1.rows; 
	cout << "Scaled: " << sz << endl;
	resize(bin_img1, bin_img1_copy, Size((int)(bin_img1.cols*sz), (int)(bin_img1.rows*sz)));
	return bin_img1_copy;
}

Mat remove_borders(Mat binary_image) {
	Mat image = binary_image.clone();
	int row_st = 0; 
	int row_ed = image.rows;
	int col_st = 0;
	int col_ed = image.cols;
	//remove left border
	for(int c = 0; c < image.cols; c++) {
		int r = 0;
		while (r < image.rows) {
			if((int)image.at<uchar>(r,c) != 0) break;
			r++;
		}
		if(r < image.rows) break;
		col_st++;
	}
	//remove right border
	for(int c = image.cols -1; c >= 0; c--) {
		int r = 0;
		while (r < image.rows) {
			if((int)image.at<uchar>(r,c) != 0) break;
			r++;
		}
		if(r < image.rows) break;
		col_ed--;
	}
	//remove top border
	for(int r=0; r<image.rows; r++) {
		int c = col_st;
		while(c < col_ed) {
			if((int)image.at<uchar>(r,c) != 0) break;
			c++;
		}
		if(c < col_ed) break;
		row_st++;
	}
	//remove bottom border
	for(int r=image.rows-1; r >= 0; r--) {
		int c = col_st;
		while(c < col_ed) {
			if((int)image.at<uchar>(r,c) != 0) break;
			c++;
		}
		if(c < col_ed) break;
		row_ed--;
	}
	Mat new_image(row_ed - row_st, col_ed - col_st, CV_8UC1, Scalar(0));

	for(int i=row_st; i < row_ed; i++) {
		for(int j=col_st; j < col_ed; j++) {
			new_image.at<uchar>(i - row_st, j - col_st) = (int)image.at<uchar>(i, j);
		}
	}
	return new_image;
}

void preprocessing_tests() {
	Mat image = imread("glyphs/_m.png", CV_LOAD_IMAGE_COLOR);
	Mat gray_image;
 	cvtColor( image, gray_image, COLOR_BGR2GRAY );
 	namedWindow( "Gray Scale", WINDOW_AUTOSIZE);
 	imshow( "Gray Scale", gray_image);

 	waitKey(0);

 	Mat bin_img = binary(gray_image, 100);
	namedWindow( "Binary Image", WINDOW_AUTOSIZE);
 	imshow( "Binary Image", bin_img);

 	waitKey(0);

 	Mat bin_img_removed_borders = negative(remove_borders(negative(bin_img)));
	namedWindow( "Binary Image Removed Borders", WINDOW_AUTOSIZE);
 	imshow( "Binary Image Removed Borders", bin_img_removed_borders);

 	waitKey(0); 	

 	Mat image2 = imread("glyphs/B.png", CV_LOAD_IMAGE_COLOR);
	Mat gray_image2;
 	cvtColor( image2, gray_image2, COLOR_BGR2GRAY );

 	Mat bin_img2 = binary(gray_image2, 100);
	namedWindow( "Binary Image 2", WINDOW_AUTOSIZE);
 	imshow( "Binary Image 2", bin_img2);

 	waitKey(0);

 	Mat bin_img2_removed_borders = negative(remove_borders(negative(bin_img2)));

 	Mat scaled = scale_image(bin_img_removed_borders, bin_img2_removed_borders, 'y');
 	namedWindow( "Scaled Image", WINDOW_AUTOSIZE);
 	imshow( "Scaled Image", scaled);

 	waitKey(0);

 	float p_overlap = percentage_overlap(negative(bin_img), negative(bin_img2));
 	cout << "percentage overlap " << p_overlap << endl;
}


float percentage_overlap(Mat bin_img1, Mat bin_img2) {
	float p_overlap = 0;
	int total_pixels = 0;
	int nrows = min(bin_img1.rows, bin_img2.rows);
	int ncols = min(bin_img1.cols, bin_img2.cols);
	for(int i=0; i< nrows; i++) {
		for(int j=0; j < ncols; j++) {
			if((int)bin_img1.at<uchar>(i,j) == 255) total_pixels++;
			if((int)bin_img1.at<uchar>(i,j) == 255 && (int)bin_img2.at<uchar>(i,j) == 255) p_overlap++;
		}
	}
	p_overlap = p_overlap / total_pixels;
	return p_overlap;
}











