#include "FuncDeclaration.h"

Mat HoleFilling(Mat object)
{
	Mat gray;
	cvtColor(object, gray, CV_BGR2GRAY);
	Mat image_thresh;
	threshold(gray, image_thresh, 125, 255, THRESH_BINARY);

	// Loop through the border pixels and if they're black, floodFill from there
	Mat mask;
	image_thresh.copyTo(mask);
	for (int i = 0; i < mask.cols; i++) {
		if (mask.at<char>(0, i) == 0)
			floodFill(mask, Point(i, 0), 255, 0, 10, 10);
		if (mask.at<char>(mask.rows - 1, i) == 0)
			floodFill(mask, Point(i, mask.rows - 1), 255, 0, 10, 10);
	}
	for (int i = 0; i < mask.rows; i++) {
		if (mask.at<char>(i, 0) == 0)
			floodFill(mask, Point(0, i), 255, 0, 10, 10);
		if (mask.at<char>(i, mask.cols - 1) == 0)
			floodFill(mask, Point(mask.cols - 1, i), 255, 0, 10, 10);
	}
	Mat holefilled = Mat::zeros(object.size(), 16);
	holefilled.setTo(Scalar(255, 255, 255));
	// Compare mask with original.
	for (int row = 0; row < mask.rows; ++row)
	for (int col = 0; col < mask.cols; ++col)
	if (mask.at<char>(row, col) == 0 || object.at<Vec3b>(row, col) == Vec3b(255, 255, 255))
		holefilled.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
	return holefilled;
}
void saveImage(string file, int index, Mat src){
	file.append(int2str(index));
	file.append(".jpg");
	imwrite(file, src);
}
string int2str(int &i) {
	string s;
	stringstream ss(s);
	ss << i;
	return ss.str();
}

extern Mat img;
char window_name[30] = "HSV Segmentation";
void ColorDisplay(){		
	imshow(window_name, img);
	setMouseCallback(window_name, onMouse, 0);
	waitKey();
}
static void onMouse(int event, int x, int y, int f, void*){
	Mat image = img.clone();
	Vec3b rgb = image.at<Vec3b>(y, x);
	int B = rgb.val[0];
	int G = rgb.val[1];
	int R = rgb.val[2];

	Mat HSV;
	Mat RGB = image(Rect(x, y, 1, 1));
	cvtColor(RGB, HSV, CV_BGR2HSV);

	Vec3b hsv = HSV.at<Vec3b>(0, 0);
	int H = hsv.val[0];
	int S = hsv.val[1];
	int V = hsv.val[2];

	char name[30];
	sprintf(name, "B=%d", B);
	putText(image, name, Point(150, 40), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

	sprintf(name, "G=%d", G);
	putText(image, name, Point(150, 80), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

	sprintf(name, "R=%d", R);
	putText(image, name, Point(150, 120), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

	sprintf(name, "H=%d", H);
	putText(image, name, Point(25, 40), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

	sprintf(name, "S=%d", S);
	putText(image, name, Point(25, 80), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

	sprintf(name, "V=%d", V);
	putText(image, name, Point(25, 120), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

	sprintf(name, "X=%d", x);
	putText(image, name, Point(25, 300), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 0, 255), 2, 8, false);

	sprintf(name, "Y=%d", y);
	putText(image, name, Point(25, 340), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 0, 255), 2, 8, false);

	//imwrite("hsv.jpg",image);
	imshow(window_name, image);
}