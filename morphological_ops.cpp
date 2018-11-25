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
	Kernel(int r, int w, int height, int width) {
		cr = r;
		cc = w;
		data.resize(height);
		for(int i=0; i < height; i++)
			data[i].resize(width);
	}
	int width() {
		if(data.size() == 0) return 0;
		return data[0].size();
	}
	int height() {
		return data.size();
	}
};
struct Coord {
	int r = 0;
	int c = 0;
	Coord(int row, int col) {
		r = row;
		c = col;
	};
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

double mean(vector<int> vals) {
	double ret_val = 0;
	for(int i=0; i<vals.size(); i++) ret_val += vals[i];
	ret_val = ret_val/(double)vals.size();
	return ret_val;
}

int count_regions(Mat binary_image);
void test();


Mat binary(Mat image, int thresh) {
	//above threshold -> 1, below -> 0
	Mat binary = image.clone();
	for(int i=0; i<binary.rows; i++) {
		for(int j=0; j<binary.cols; j++) {
			binary.at<uchar>(i, j) = (int)binary.at<uchar>(i, j) >= thresh ? 255 : 0;
		}
	}
	return binary;
}

Mat negative(Mat image) {
	Mat neg = image.clone();
	int transformed[256];
	for(int i=0; i<neg.rows; i++) {
		for(int j=0; j<neg.cols; j++) {
			transformed[(int)neg.at<uchar>(i, j)] = 255 - (int)neg.at<uchar>(i, j);
			neg.at<uchar>(i, j) = 255 - (int)neg.at<uchar>(i, j);
		}
	}
	return neg;
}

Mat erosion(Mat binary_image, Kernel k) {
	Mat return_val(binary_image.rows, binary_image.cols, CV_8U);
	for(int i=0; i < return_val.rows; i++) {
		for(int j=0; j < return_val.cols; j++) return_val.at<uchar>(i, j) = 0;
	}
	int num_1s = 0;
	for(int h=0; h<k.height(); h++ ) {
		for(int w=0; w<k.width(); w++) {
			num_1s += (int)k.data[h][w];
		}
	}
	for(int i=0; i <= binary_image.rows - k.height(); i++) {
		for(int j=0; j <= binary_image.cols - k.width(); j++) {

			int sum = 0;
			for(int h=0; h<k.height(); h++ ) {
				for(int w=0; w<k.width(); w++) {
					int overlap = k.data[h][w] * (int)binary_image.at<uchar>(i+h, j+w);
					// cout << "overlap: " << overlap << endl;
					sum += overlap;
				}
			}
			// cout << "Sum: "  << sum << endl;
			if(sum == (255*num_1s)) {	//kernel fully overlaps
				return_val.at<uchar>(k.cr + i, k.cc + j) = 255;
			}
		}
	}
	return return_val;
}
Mat erosion_iter(Mat binary_image, Kernel k, int iter) {
	Mat eroded = erosion(binary_image, k);
	for(int i=0; i < iter-1; i++) eroded = erosion(eroded, k);
	return eroded;
}
Mat dilation_iter(Mat binary_image, Kernel k, int iter) {
	Mat dilated = dilation(binary_image, k);
	for(int i=0; i < iter-1; i++) dilated = dilation(dilated, k);
	return dilated;
}
Mat dilation(Mat binary_image, Kernel k) {
	Mat return_val(binary_image.rows, binary_image.cols, CV_8U);
	for(int i=0; i < return_val.rows; i++) {
		for(int j=0; j < return_val.cols; j++) return_val.at<uchar>(i, j) = 0;
	}
	for(int i=0; i <= binary_image.rows - k.height(); i++) {
		for(int j=0; j <= binary_image.cols - k.width(); j++) {

			int sum = 0;
			for(int h=0; h<k.height(); h++ ) {
				for(int w=0; w<k.width(); w++) {
					int overlap = k.data[h][w] * (int)binary_image.at<uchar>(i+h, j+w);
					// cout << "overlap: " << overlap << endl;
					sum += overlap;
				}
			}
			// cout << "Sum: "  << sum << endl;
			if(sum >= 255) {	//kernel fully overlaps
				return_val.at<uchar>(k.cr + i, k.cc + j) = 255;
			}
		}
	}
	return return_val;
}
Mat closing(Mat binary_image, Kernel k) {
	Mat dilated = dilation(binary_image, k);
	Mat dilated_eroded = erosion(dilated, k);
	return dilated_eroded;
}
Mat opening(Mat binary_image, Kernel k) {
	Mat eroded = erosion(binary_image, k);
	Mat eroded_dilated = dilation(eroded, k);
	return eroded_dilated;
}
Mat erosion_grayscale(Mat gray_image, Kernel k) {
	Mat return_val(gray_image.rows, gray_image.cols, CV_8U);
	for(int i=0; i < return_val.rows; i++) {
		for(int j=0; j < return_val.cols; j++) return_val.at<uchar>(i, j) = 0;
	}
	for(int i=0; i <= gray_image.rows - k.height(); i++) {
		for(int j=0; j <= gray_image.cols - k.width(); j++) {

			int min_val = 255;
			for(int h=0; h<k.height(); h++ ) {
				for(int w=0; w<k.width(); w++) {
					int overlap = (int)gray_image.at<uchar>(i+h, j+w) - k.data[h][w];
					if(overlap < 0) overlap = 0;
					if(overlap > 255) overlap = 255;
					if(overlap < min_val) min_val = overlap;
				}
			}
			return_val.at<uchar>(k.cr + i, k.cc + j) = min_val;
		}
	}
	return return_val;
}
Mat dilation_grayscale(Mat gray_image, Kernel k) {
	Mat return_val(gray_image.rows, gray_image.cols, CV_8U);
	for(int i=0; i < return_val.rows; i++) {
		for(int j=0; j < return_val.cols; j++) return_val.at<uchar>(i, j) = 0;
	}
	for(int i=0; i <= gray_image.rows - k.height(); i++) {
		for(int j=0; j <= gray_image.cols - k.width(); j++) {

			int max_val = 0;
			for(int h=0; h<k.height(); h++ ) {
				for(int w=0; w<k.width(); w++) {
					int overlap = (int)gray_image.at<uchar>(i+h, j+w) +k.data[h][w];
					if(overlap < 0) overlap = 0;
					if(overlap > 255) overlap = 255;
					if(overlap > max_val) max_val = overlap;
				}
			}
			return_val.at<uchar>(k.cr + i, k.cc + j) = max_val;
		}
	}
	return return_val;
}
Mat erosion_grayscale_iter(Mat gray_image, Kernel k, int iter) {
	Mat eroded = erosion_grayscale(gray_image, k);
	for(int i=0; i < iter-1; i++) eroded = erosion_grayscale(eroded, k);
	return eroded;
};
Mat dilation_grayscale_iter(Mat gray_image, Kernel k, int iter) {
	Mat dilated = dilation_grayscale(gray_image, k);
	for(int i=0; i < iter-1; i++) dilated = dilation_grayscale(dilated, k);
	return dilated;
};
Mat closing_grayscale(Mat gray_image, Kernel k) {
	Mat dilated = dilation_grayscale(gray_image, k);
	Mat dilated_eroded = erosion_grayscale(dilated, k);
	return dilated_eroded;
};
Mat opening_grayscale(Mat gray_image, Kernel k) {
	Mat eroded = erosion_grayscale(gray_image, k);
	Mat eroded_dilated = dilation_grayscale(eroded, k);
	return eroded_dilated;	
};
void test() {
	cout << "Performing Tests" << endl;
	
	Mat image = imread("TestImage-even-width.bmp", CV_LOAD_IMAGE_COLOR);
	Mat gray_image;
 	cvtColor( image, gray_image, COLOR_BGR2GRAY );
 	namedWindow( "Gray Scale", WINDOW_AUTOSIZE);
 	imshow( "Gray Scale", gray_image);

 	Mat binary_image = binary(gray_image, 190);
 	namedWindow( "Binary Image", WINDOW_AUTOSIZE);
 	imshow( "Binary Image", binary_image);
 	waitKey(0);

 // 	Mat neg = negative(gray_image);
	// namedWindow( "Negative Image", WINDOW_AUTOSIZE);
 // 	imshow( "Negative Image", neg);
 // 	waitKey(0);

 // 	Mat binary_image = binary(neg, 65);
 // 	namedWindow( "Binary Image", WINDOW_AUTOSIZE);
 // 	imshow( "Binary Image", binary_image);
 // 	waitKey(0);

 	//basic kernel
 	// Kernel erosion_kernel1(1, 1, 3, 3);
 	// int erosion_data_2D_array[3][3] = {{0,1,0}, {1,1,1}, {0,1,0}};
 	// for(int i=0; i< 3; i++) {
 	// 	for(int j=0; j< 3; j++) {
 	// 		erosion_kernel1.data[i][j] = erosion_data_2D_array[i][j];
 	// 	}
 	// }
 	Kernel erosion_kernel1(2, 2, 5, 5);
 	int erosion_data_2D_array[5][5] = {{0,1,1,1,0}, {0,1,1,1,0}, {1,1,1,1,1}, {0,1,1,1,0}, {0,1,1,1,0}};
 	for(int i=0; i< 5; i++) {
 		for(int j=0; j< 5; j++) {
 			erosion_kernel1.data[i][j] = erosion_data_2D_array[i][j];
 		}
 	}
 	Kernel grayscale_kernel1(1, 1, 3, 3);
 	int erosion_data_2D_array_grayscale[3][3] = {{50,50,50}, {50,50,50}, {50,50,50}};
 	for(int i=0; i< 3; i++) {
 		for(int j=0; j< 3; j++) {
 			grayscale_kernel1.data[i][j] = erosion_data_2D_array[i][j];
 		}
 	}

 	Mat grayscale_eroded = erosion_grayscale(gray_image, grayscale_kernel1);
 	namedWindow( "Grayscale Eroded", WINDOW_AUTOSIZE);
 	imshow( "Grayscale Eroded", grayscale_eroded);
 	waitKey(0);


 	Mat grayscale_dilated = dilation_grayscale(gray_image, grayscale_kernel1);
 	namedWindow( "Grayscale dilated", WINDOW_AUTOSIZE);
 	imshow( "Grayscale dilated", grayscale_dilated);
 	waitKey(0);

 	Mat eroded = erosion_iter(binary_image, erosion_kernel1, 1);
	namedWindow( "Binary Eroded Image", WINDOW_AUTOSIZE);
 	imshow( "Binary Eroded Image", eroded);
 	waitKey(0); 

 	Mat dilated = dilation_iter(binary_image, erosion_kernel1, 1);
	namedWindow( "Binary Dilated Image", WINDOW_AUTOSIZE);
 	imshow( "Binary Dilated Image", dilated);
 	waitKey(0); 

 	// int basic_image[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
 	// int basic_image[5][5] = {{1,1,1,0,0},{1,1,1,1,0},{1,1,1,1,0},{0,1,1,1,0},{0,0,0,0,0}};
 	// Mat basic_mat(5, 5, CV_8U);
 	//test erosion
 	// for(int i=0; i < 5; i++) {
 	// 	for(int j=0; j < 5; j++) basic_mat.at<uchar>(i, j) = basic_image[i][j]*255;
 	// }
 	// Mat eroded = erosion(basic_mat, erosion_kernel1);
 	// for(int i=0; i < 5; i++) {
 	// 	for(int j=0; j < 5; j++) {cout << (int)eroded.at<uchar>(i, j) << " ";}
		// cout << endl;
 	// }
 	// Mat dilated = dilation(basic_mat, erosion_kernel1);
 	// for(int i=0; i < 5; i++) {
 	// 	for(int j=0; j < 5; j++) {cout << (int)dilated.at<uchar>(i, j) << " ";}
		// cout << endl;
 	// }
 	Mat opened = closing(binary_image, erosion_kernel1);
 	for(int i=0; i < 10; i++) opened = opening(opened, erosion_kernel1);
 	namedWindow( "opened Image", WINDOW_AUTOSIZE);
 	imshow( "opened Image", opened);
 	waitKey(0); 
 	Mat opened_closed = opening(opened, erosion_kernel1);
 	for(int i=0; i < 10; i++) opened_closed = opening(opened_closed, erosion_kernel1);
 	namedWindow( "opened_closed Image", WINDOW_AUTOSIZE);
 	imshow( "opened_closed Image", opened_closed);
 	waitKey(0); 
}

vector<int> connected_components(Mat image) {
	Mat new_image = image.clone();
	bool** trav = new bool*[new_image.rows];
	int** region = new int*[new_image.rows];
	for(int i=0; i < new_image.rows; i++) {
		region[i] = new int[new_image.cols];
		trav[i] = new bool[new_image.cols];
	}
	for(int i=0; i<new_image.rows; i++) {
		for(int j=0; j<new_image.cols; j++) {
			region[i][j] = -1;
		}
	}
	for(int i=0; i<new_image.rows; i++) {
		for(int j=0; j<new_image.cols; j++) {
			trav[i][j] = false;
		}
	}
	int sz = 0;
	std::vector<int> sizes;
	for(int i=0; i<new_image.rows; i++) {
		for(int j=0; j<new_image.cols; j++) {
			sz = bfs(new_image, trav, Coord(i,j), sizes.size(), region);
			if(sz > 0) sizes.push_back(sz);
		}
	}
	return sizes;
}
int bfs(Mat image, bool** traversed, Coord pix, int reg, int** region) {
	//assume image is binary
	std::queue<Coord> q;
	q.push(pix);
	int sz = 0;
	while(!q.empty()) {
		Coord pix = q.front();
		q.pop();
		if(pix.r < 0 || pix.r >= image.rows || pix.c < 0 || pix.c >= image.cols) continue;
		if(!traversed[pix.r][pix.c] && (int)image.at<uchar>(pix.r, pix.c) == 255) {
			traversed[pix.r][pix.c] = true;
			region[pix.r][pix.c] = reg;
			sz++;
			//check the 4 neighbors
			q.push(Coord(pix.r + 1, pix.c));
			q.push(Coord(pix.r - 1, pix.c));
			q.push(Coord(pix.r, pix.c + 1));
			q.push(Coord(pix.r, pix.c - 1));
		};
	}
	return sz;
}

/*
int main() {
	// test();
	//actual algorithm
	Mat image = imread("TestImage-even-width.bmp", CV_LOAD_IMAGE_COLOR);
	Mat gray_image;
 	cvtColor( image, gray_image, COLOR_BGR2GRAY );
 	namedWindow( "Gray Scale", WINDOW_AUTOSIZE);
 	imshow( "Gray Scale", gray_image);

 	Mat binary_image = binary(gray_image, 200);
 	namedWindow( "Binary Image", WINDOW_AUTOSIZE);
 	imshow( "Binary Image", binary_image);
 	waitKey(0);

 	Kernel kernel1(2, 2, 5, 5);
 	int data_k1[5][5] = {{0,0,1,0,0}, {0,0,1,0,0}, {1,1,1,1,1}, {0,0,1,0,0}, {0,0,1,0,0}};
 	for(int i=0; i< 5; i++) {
 		for(int j=0; j< 5; j++) {
 			kernel1.data[i][j] = data_k1[i][j];
 		}
 	}

 	Kernel kernel4(2, 2, 5, 5);
 	int data_k4[5][5] = {{1,1,1,1,1}, {1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}};
 	for(int i=0; i< 5; i++) {
 		for(int j=0; j< 5; j++) {
 			kernel4.data[i][j] = data_k4[i][j];
 		}
 	}
 	Kernel kernel3(1,1,2,2);
 	int data_k3[2][2] = {{1,1}, {1,1}};
 	for(int i=0; i< 2; i++) {
 		for(int j=0; j< 2; j++) {
 			kernel3.data[i][j] = data_k3[i][j];
 		}
 	}
 	Kernel kernel2(1, 1, 3, 3);
 	int data_k2[3][3] = {{1,1,1}, {1,1,1}, {1,1,1}};
 	for(int i=0; i< 3; i++) {
 		for(int j=0; j< 3; j++) {
 			kernel2.data[i][j] = data_k2[i][j];
 		}
 	}
 	Mat final_image = binary_image;
 	// final_image = erosion_iter(final_image, kernel3, 5);
 	final_image = negative(final_image);
 	final_image = dilation_iter(final_image, kernel3, 3);

 	// final_image = erosion_iter(final_image, kernel1, 4);
 	final_image = negative(final_image);
 	// final_image = negative(final_image);
 	// final_image = erosion_iter(final_image, kernel4, 10);
 	// final_image = negative(final_image);
 	// final_image = dilation_iter(erosion_iter(final_image, kernel2, 4), kernel1, 1);
 	// final_image = dilation_iter(erosion_iter(final_image, kernel2, 4), kernel1, 1);
 	// final_image = dilation_iter(erosion_iter(final_image, kernel2, 2), kernel1, 1);
 	// final_image = negative(final_image);
 	// final_image = erosion_iter(final_image, kernel1, 2);
 	final_image = closing(final_image, kernel3);
 	final_image = closing(final_image, kernel3);
 	final_image = dilation_iter(final_image, kernel3, 3);
 	namedWindow( "Final Image", WINDOW_AUTOSIZE);
 	imshow( "Final Image", final_image);
 	waitKey(0); 
 	vector<int> region_szs = connected_components(final_image);
 	for(int i=0; i < region_szs.size(); i++) cout << region_szs[i] << " ";
 	cout << endl;
 	cout << "mean: " << mean(region_szs) << endl;
 	double mu = mean(region_szs);
 	int regions = 0;
 	for(int i=0; i < region_szs.size(); i++) {
 		if(region_szs[i] >= mu*(0.10)) regions += 1;
 	}
 	cout << "# of objects found: " << regions << endl;

 	//morphological operations on the grayscale image:
 	Mat final_grayscale = gray_image;
 	final_grayscale = negative(final_grayscale);
 	final_grayscale = dilation_grayscale_iter(final_grayscale, kernel3, 3);
 	final_grayscale = negative(final_grayscale);
 	final_grayscale = closing_grayscale(final_grayscale, kernel3);
 	final_grayscale = closing_grayscale(final_grayscale, kernel3);
 	final_grayscale = dilation_grayscale_iter(final_grayscale, kernel3, 3);
 	namedWindow( "Final Grayscale", WINDOW_AUTOSIZE);
 	imshow( "Final Grayscale", final_grayscale);
 	waitKey(0); 
	return 0;
}

*/




