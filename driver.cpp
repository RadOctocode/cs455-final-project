#include <iostream>
#include "deskew.cpp"
#include "extract.cpp"
#include "pattern_matching.cpp"
#include <chrono>
// #include "morphological_ops.hpp"

using namespace std;

/*
string classify(Mat bin_img) {

	string best_match = "";
	Kernel erosion_kernel1(1, 1, 3, 3);
 	int erosion_data_2D_array[3][3] = {{0,1,0}, {1,1,1}, {0,1,0}};
 	for(int i=0; i< 3; i++) {
 		for(int j=0; j< 3; j++) {
 			erosion_kernel1.data[i][j] = erosion_data_2D_array[i][j];
 		}
 	}

	string glyphs_dir = "glyphs";
	vector<string> glyphs;
	read_directory(glyphs_dir, glyphs);
	for(int i=0; i < glyphs.size(); i++) {
		// cout << glyphs[i] << endl;
	}
	imshow("Character", bin_img);
	waitKey(0);
	// Mat skeletoned = skeleton(bin_img, erosion_kernel1);
	Mat scaled;
	// imshow("skeletoned", skeletoned);
	waitKey(0);

	float percent = 0;
	for(int i=0; i < glyphs.size(); i++) {
		if(glyphs[i] != "." && glyphs[i] != ".." && glyphs[i] != ".DS_Store") {
			Mat image = imread("glyphs/" + glyphs[i]);

			Mat gray_image;
	 		cvtColor( image, gray_image, COLOR_BGR2GRAY );
	 		Mat glyph_bin = negative(binary(gray_image, 200));
	 		glyph_bin = remove_borders(glyph_bin);		//negative

	 		scaled = scale_image(bin_img, glyph_bin, 'y');
	 		Mat skeletoned = skeleton(scaled, erosion_kernel1);

	 		imshow("scaled", scaled);

	 		
			float val = percentage_overlap(skeletoned, glyph_bin);
			imshow("skeletoned", skeletoned);
			if (val > percent) {
				percent = val;
				// if(glyphs[i] == "comma.png") best_match = ',';
				// else if(glyphs[i] == "exclamation.png") best_match = '!';
				// else if(glyphs[i] == "period.png") best_match = '.';
				// else if(glyphs[i] == "question_mark.png") best_match = '?';
				// else if(glyphs[i] == "semicolon.png") best_match = ';';
				// else if(glyphs[i][0] == '_') best_match = glyphs[i][1];
				// else best_match = glyphs[i][0];
				best_match = glyphs[i];
			}
		}
	}	
	cout << percent << endl;
	cout << best_match << endl;
	return best_match;
}

*/


//Timer class taken from here: https://stackoverflow.com/questions/728068/how-to-calculate-a-time-difference-in-c
class Timer
{
public:
    Timer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const { 
        return std::chrono::duration_cast<second_>
            (clock_::now() - beg_).count(); }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> beg_;
};

int main(int argc, char ** argv) {
	
	Mat deskewed = binary(negative(compute_skew(imread(argv[1], 0))), 50);
	imshow("Deskewed", deskewed);
	waitKey(0);
	imwrite("deskewed.jpg", deskewed);
	
	vector<Mat> images = extract("deskewed.jpg");
	for(int i=0; i < images.size(); i++) {
		imshow("images", images[i]);
		waitKey(0);
	}
	
	vector<vector<Mat>> characters;
	get_individual_characters(images, characters);

	for(int i=0; i < characters.size(); i++) {
		cout << "New Word: " << endl;
		for(int j=0; j < characters[i].size(); j++) {
			cout << characters[i][j].rows << " " << characters[i][j].cols << endl;
			imshow("CHARACTER", characters[i][j]);
			waitKey(0);
		}
	}
	
	Timer tmr;
	tmr.reset();
	for(int i=0; i < characters.size(); i++) {
		for(int j=0; j < characters[i].size(); j++) {
		 	// classify(characters[i][j]);
		 	cout << classify(characters[i][j]);
		 	// imshow("Char", characters[i][j]);
		 	// waitKey(0);
		}
		// cout << "New Word: " << endl;
		cout << " ";
	}
	cout << endl;
	cout << tmr.elapsed() << " seconds"<< endl;
	return 0;
}

//comparison to tesseract ...




