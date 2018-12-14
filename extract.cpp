#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <queue>
#include <vector>

using namespace cv;
using namespace std;
int histogram[256] = {0};

//generate histogram int array given image
void createHistogram(Mat img){

	for(int x = 0; x < 256; x++){
		histogram[x] = 0;
	}
	
	for(int x = 0; x < img.rows; x++){
		for(int y = 0; y < img.cols; y++){
			histogram[img.at<uchar>(x,y)] += 1;
		}
	}
}
vector<int> connected_comp(Mat& img, int x, int y, uchar set_color, uchar change_color){

    queue<int> queue_x;
    queue<int> queue_y;
    queue_x.push(x);
    queue_y.push(y);

    int pixelCount = 0;

    vector<int> matrix;
    int min_x, min_y, max_x, max_y;
    min_x  = img.rows;
    min_y  = img.cols;
    max_x  = -1;
    max_y  = -1;
    

    while(!queue_x.empty() && !queue_y.empty()) {
        int i = queue_x.front();
        int j = queue_y.front();
        queue_x.pop();
        queue_y.pop();

        // Check if visited before continuing
        if(img.at<uchar>(i,j) != set_color){

		if(i < min_x){
			min_x = i;
		}
		if(j < min_y){
			min_y = j;
		}
		if(i > max_x){
			max_x = i;
		}
		if(j > max_y){
			max_y = j;
		}
		
		//increase size of found region
		pixelCount++;

		//set as visited
		img.at<uchar>(i,j) = set_color;

		//find surrounding components that have not been visited
		if(j+1 < img.cols && img.at<uchar>(i,j+1) == change_color){
			queue_x.push(i);
			queue_y.push(j+1);
		}
		if(j-1 >= 0 && img.at<uchar>(i,j-1) == change_color){
			queue_x.push(i);	
			queue_y.push(j-1);
		}
		if(i+1 < img.rows && img.at<uchar>(i+1,j) == change_color){
			queue_x.push(i+1);
			queue_y.push(j);
		}
		if(i-1 >= 0 && img.at<uchar>(i-1,j) == change_color){
			queue_x.push(i-1);
			queue_y.push(j);
		}
	}
    }
  
    matrix.push_back(pixelCount);
    matrix.push_back(min_x);
    matrix.push_back(min_y);
    matrix.push_back(max_x);
    matrix.push_back(max_y);
 
    //return size 
    //return pixelCount;
    return matrix;
}
//create binary image
Mat createBinaryImage(Mat img){
	int total_pixels = img.rows * img.cols;
	createHistogram(img);
	int threshold = 0;
	
	//find threshold by calculating the averages of pixels
	for(int x = 0; x < 256; x++){
		threshold += x * histogram[x];
	}
	threshold = threshold/total_pixels;
	cout << "Threshold Value: " << threshold << endl;
	
	Mat bin_img = img.clone();
	for(int x = 0; x < bin_img.rows; x++){
		for(int y = 0; y < bin_img.cols; y++){
			if(bin_img.at<uchar>(x,y) < threshold)
				bin_img.at<uchar>(x,y) = 0;
			else
				bin_img.at<uchar>(x,y) = 255;
				
		}
	}
	
	return bin_img;
}
vector<Mat> regionDetection(Mat& img){
	int min_size = INT_MAX;
	int max_size = 0;
	int min_x = -1;
	int min_y = -1;
	int max_x = -1;
	int max_y = -1;

	vector<Mat> outs;
	//color all regions with medium color (120)
	//find the smallest and largest regions
	for(int x = 0; x < img.rows; x++){
		for(int y = 0; y < img.cols; y++){
			if(img.at<uchar>(x,y) == 255){
				vector<int> matri = connected_comp(img, x, y, 120, 255);
				int total_pixels = matri.at(0);
				int diff_x = matri.at(3) - matri.at(1);
				int diff_y = matri.at(4) - matri.at(2);	
				Rect roi = Rect(matri.at(2), matri.at(1), diff_y, diff_x);
				if(roi.x >= 0 && roi.y >= 0 && roi.width + roi.x < img.cols && roi.height + roi.y < img.rows){
					Mat cropped = img(roi);
					Mat bin_img_ret;
					threshold(cropped, bin_img_ret, 0.0, 255.0, THRESH_BINARY);
				//	namedWindow("Ret Image", WINDOW_AUTOSIZE);
				//	imshow("Ret Image", cropped);
				//	waitKey();
				//	destroyAllWindows();
					outs.push_back(bin_img_ret);
				}

			
				if(total_pixels < min_size){
					min_size = total_pixels;
					min_x = x;
					min_y = y;
				}else if(total_pixels > max_size){
					max_size = total_pixels;
					max_x = x;
					max_y = y;
				}

			}	
		}
	}

	//color smallest and largest regions	
	vector<int> matri = connected_comp(img, min_x, min_y, 60, 120);
	int diff_x = matri.at(3) - matri.at(1);
	int diff_y = matri.at(4) - matri.at(2);	
	Range cols(matri.at(1), diff_x);
	Range rows(matri.at(3), diff_y);
	
	Rect roi = Rect(matri.at(2), matri.at(1), diff_y, diff_x);
	
	if(roi.x >= 0 && roi.y >= 0 && roi.width + roi.x < img.cols && roi.height + roi.y < img.rows){
		Mat cropped = img(roi);
	//	outs.push_back(cropped);
	//	namedWindow("Ret Image", WINDOW_AUTOSIZE);
	//	imshow("Ret Image", cropped);
	//	waitKey();
	//	destroyAllWindows();
	}

	matri = connected_comp(img, max_x, max_y, 200, 120);
	diff_x = matri.at(3) - matri.at(1);
	diff_y = matri.at(4) - matri.at(2);	
	roi = Rect(matri.at(2), matri.at(1), diff_y, diff_x);
	if(roi.x >= 0 && roi.y >= 0 && roi.width + roi.x < img.cols && roi.height + roi.y < img.rows){
		Mat cropped = img(roi);
	//	outs.push_back(cropped);
	//	namedWindow("Ret1 Image", WINDOW_AUTOSIZE);
	//	imshow("Ret1 Image", cropped);
	//	waitKey();
	//	destroyAllWindows();
	}
	
	return outs;
}

vector<Mat> extract(string input_image) {
	// string input_image = argv[1];
	vector<Mat> outs;
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
			Mat cropped = out_img(rect);
			outs.push_back(cropped);
		}
		contour_index = hierarchy[contour_index][0];
	}

	imwrite("final.jpg", out_img);	
	
	// waitKey();
	// destroyAllWindows();
	return outs;
}

vector<vector<Mat>> get_individual_characters(vector<Mat> images){
	vector<vector<Mat>> characters_words;

	for(int x = 0; x < images.size(); x++){
		Mat src_img = images[x];
		Mat grey;
		cvtColor(src_img, grey, CV_BGR2GRAY);
		Mat bin_img;
		threshold(grey, bin_img, 0.0, 255.0, THRESH_BINARY);
		namedWindow("Bin Image", WINDOW_AUTOSIZE);
		imshow("Bin Image", bin_img);
		bitwise_not(bin_img, bin_img);
		namedWindow("Bin2 Image", WINDOW_AUTOSIZE);
		imshow("Bin2 Image", bin_img);
		vector<Mat> ret_img = regionDetection(bin_img);
		characters_words.push_back(ret_img);
	}
	int count = 0;
	for(int i = 0; i < characters_words.size(); i++){
		cout << characters_words[i].size() << endl;
		vector<Mat> ret_img = characters_words.at(i);
		for(int x = 0; x < ret_img.size(); x++){
//			Mat grey_ret = ret_img[x];
			Mat bin_img_ret = ret_img[x];
//			threshold(grey_ret, bin_img_ret, 0.0, 255.0, THRESH_BINARY);
			namedWindow("Ret" + to_string(count), WINDOW_AUTOSIZE);
			imshow("Ret" + to_string(count), bin_img_ret);
			imwrite("Ret" + to_string(count) + ".jpg", bin_img_ret);	
			count++;	
		}
	}
	
	return characters_words;
}
int main(int argc, char* argv[]){
	string input(argv[1]);
	vector<Mat> images = extract(input);
	for(int i=0; i < images.size(); i++) {
		namedWindow("Cropped" + to_string(i), WINDOW_AUTOSIZE);
		imshow("Cropped" + to_string(i), images[i]);
		imwrite("Cropped" + to_string(i) + ".jpg", images[i]);	
		cout << "x, y: " << images[i].rows << " " << images[i].cols << endl;
	}
	
	vector<vector<Mat>> chars = get_individual_characters(images);
	cout << chars[2].size() << endl;
	
	int count = 0;
	for(int x = 0; x < chars.size(); x++){
		for(int y = 0; y < chars[x].size(); y++){
			namedWindow("chars" + to_string(count), WINDOW_AUTOSIZE);
			imshow("chars" + to_string(count), chars[x][y]);
			count++;
		}
	}

	waitKey();
	destroyAllWindows();
	return 0;
}



