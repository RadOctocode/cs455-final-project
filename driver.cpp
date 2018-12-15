#include <iostream>
#include "deskew.cpp"
#include "extract.cpp"
#include "pattern_matching.cpp"
#include "morphological_ops.hpp"
#include <sys/types.h>
#include <dirent.h>

using namespace std;

void read_directory(const std::string& name, vector<string>& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        v.push_back(dp->d_name);
    }
    closedir(dirp);
}

char classify(Mat bin_img) {

	char best_match = '1';
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
	Mat skeletoned = skeleton(bin_img, erosion_kernel1);
	Mat scaled;
	imshow("skeletoned", skeletoned);
	waitKey(0);

	float percent = 0;
	for(int i=0; i < glyphs.size(); i++) {
		if(glyphs[i] != "." && glyphs[i] != ".." && glyphs[i] != ".DS_Store") {
			Mat image = imread("glyphs/" + glyphs[i]);

			Mat gray_image;
	 		cvtColor( image, gray_image, COLOR_BGR2GRAY );
	 		Mat glyph_bin = negative(binary(gray_image, 100));
	 		glyph_bin = remove_borders(glyph_bin);		//negative

	 		scaled = scale_image(skeletoned, glyph_bin, 'y');

	 		imshow("scaled", scaled);
	 		imshow("glyph_bin", glyph_bin);
			float val = percentage_overlap(scaled, glyph_bin);
			if (val > percent) {
				percent = val;
				if(glyphs[i] == "comma.png") best_match = ',';
				else if(glyphs[i] == "exclamation.png") best_match = '!';
				else if(glyphs[i] == "period.png") best_match = '.';
				else if(glyphs[i] == "question_mark.png") best_match = '?';
				else if(glyphs[i] == "semicolon.png") best_match = ';';
				else if(glyphs[i][0] == '_') best_match = glyphs[i][1];
				else best_match = glyphs[i][0];
			}
		}
	}	
	cout << best_match << endl;
	return best_match;
}


int main(int argc, char ** argv) {
	// // Mat input = imread(argv[1]);

	// // Mat deskewed_image = negative(compute_skew(input));

	// // imshow("Original", input);
	// // imshow("Rotated", deskewed_image);
	// // waitKey(0);

	// // imwrite("deskewed_image.jpg", deskewed_image);

	// vector<Mat> words = extract("deskewed_image.jpg");
	// // vector<Mat> words = extract("sample_images/helloworld.png");
	// // vector<Mat> words = extract(argv[1]);
	// // for(int i=0; i < words.size(); i++) {
	// // 	cout << i << endl;
	// // 	imshow(to_string(i), words[i]);
	// // 	waitKey(0);
	// // }
	// vector<vector<Mat>> characters;
	// get_individual_characters(words, characters);
	// for(int i=0; i < characters.size(); i++) {
	// 	for(int j=0; j < characters[i].size(); j++) {
	// 		imshow("Character", characters[i][j]);
	// 		waitKey(0);
	// 	}
	// }
	// return 0;
	
	// string input(argv[1]);

	// vector<Mat> images = extract(input);
	vector<Mat> images = extract("sample_images/hello_world_helv.png");
	// for(int i=0; i < images.size(); i++) {
	// 	namedWindow("Cropped" + to_string(i), WINDOW_AUTOSIZE);
	// 	imshow("Cropped" + to_string(i), images[i]);
	// 	imwrite("Cropped" + to_string(i) + ".jpg", images[i]);	
	// 	cout << "x, y: " << images[i].rows << " " << images[i].cols << endl;
	// }
	
	// vector<vector<Mat>> characters = get_individual_characters(images);
	
	vector<vector<Mat>> characters;
	get_individual_characters(images, characters);
	for(int i=0; i < characters.size(); i++) {
		for(int j=0; j < characters[i].size(); j++) {
		 	// classify(characters[i][j]);
		 	imshow("Char", characters[i][j]);
		 	waitKey(0);
		}
	}
	return 0;
	
}






