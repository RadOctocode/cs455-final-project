#ifndef MORPHOLOGICAL_OPS_H
#define MORPHOLOGICAL_OPS_H

#include <opencv2/opencv.hpp>
#include <opencv2/plot.hpp>
#include <cmath>
#include <vector>
#include <queue>
#include <iostream>
#include <vector>

// dilation operation
// erosion operation
// binary operation with threshold
// convolution operation
// struct object that holds area + center coordinates
// 4- connected components

#define PI 3.14159265

using namespace std;
using namespace cv;

//declarations
struct Kernel {
	vector<vector<int>> data;
	int cr = 0;	//x - coordinate of center
	int cc = 0; //y - coordinate of center
	Kernel(int r, int w, int height, int width);
	int width();
	int height();
};
struct Coord {
	int r = 0;
	int c = 0;
	Coord(int row, int col);
};
Mat binary(Mat image, int thresh);
Mat negative(Mat image);
Mat reverse(Mat binary_image);
Mat erosion(Mat binary_image, Kernel k); // sum equals (kernel-width)*(kernel-height)*255
Mat erosion_iter(Mat binary_image, Kernel k, int iter);
Mat dilation(Mat binary_image, Kernel k);	// sum >= 255
Mat dilation_iter(Mat binary_image, Kernel k, int iter);
Mat closing(Mat binary_image, Kernel k);
Mat opening(Mat binary_image, Kernel k);
Mat erosion_grayscale(Mat gray_image, Kernel k);
Mat erosion_grayscale_iter(Mat gray_image, Kernel k, int iter);
Mat dilation_grayscale(Mat gray_image, Kernel k);
Mat dilation_grayscale_iter(Mat gray_image, Kernel k, int iter);
Mat closing_grayscale(Mat gray_image, Kernel k);
Mat opening_grayscale(Mat gray_image, Kernel k);
double mean(vector<int>);		

// 4-connected component count regions
vector<int> connected_components(Mat image);
int bfs(Mat image, bool** traversed, Coord pix, int reg, int** region);

double mean(vector<int> vals);

int count_regions(Mat binary_image);
void test();

#endif



