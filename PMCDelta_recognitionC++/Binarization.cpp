#include "FuncDeclaration.h"

extern bool binaryShow;
extern bool refineShow;
extern bool save;
extern bool tune;
extern Mat img;
extern Mat binary;
extern Mat refine;

int morph_elem = 0;
int morph_size = 3;
int const max_morph_size = 10;
int const open_operator = 2;
int const close_operator = 3;
int meanH, meanS, meanV;

void findRange(Mat image, int meanH, int meanS, int meanV, int *rangeH, int *rangeS, int *rangeV){

	int max_dist = 0;
	int min_dist = 10000;

	int maxdistH = 0;
	int maxdistS = 0;
	int maxdistV = 0;

	for (int y = 0; y<image.rows; y++)
	for (int x = 0; x<image.rows; x++){
		int distH = abs((int)image.at<Vec3b>(y, x).val[0] - meanH);
		int distS = abs((int)image.at<Vec3b>(y, x).val[1] - meanS);
		int distV = abs((int)image.at<Vec3b>(y, x).val[2] - meanV);

		maxdistH = (maxdistH>distH) ? maxdistH : distH;
		maxdistS = (maxdistS>distS) ? maxdistS : distS;
		maxdistV = (maxdistV>distV) ? maxdistV : distV;
	}
	for (int y = 0; y<image.rows; y++)
	for (int x = 0; x<image.rows; x++){
		int distH = abs((int)image.at<Vec3b>(y, x).val[0] - meanH);
		int distS = abs((int)image.at<Vec3b>(y, x).val[1] - meanS);
		int distV = abs((int)image.at<Vec3b>(y, x).val[2] - meanV);

		if (distH<15 && distS<50){
			if ((int)image.at<Vec3b>(y, x).val[0]<rangeH[0])
				rangeH[0] = (int)image.at<Vec3b>(y, x).val[0];
			if ((int)image.at<Vec3b>(y, x).val[0]>rangeH[1])
				rangeH[1] = (int)image.at<Vec3b>(y, x).val[0];

			if ((int)image.at<Vec3b>(y, x).val[1]<rangeS[0])
				rangeS[0] = (int)image.at<Vec3b>(y, x).val[1];
			if ((int)image.at<Vec3b>(y, x).val[1]>rangeS[1])
				rangeS[1] = (int)image.at<Vec3b>(y, x).val[1];

			if ((int)image.at<Vec3b>(y, x).val[2]<rangeV[0])
				rangeV[0] = (int)image.at<Vec3b>(y, x).val[2];
			if ((int)image.at<Vec3b>(y, x).val[2]>rangeV[1])
				rangeV[1] = (int)image.at<Vec3b>(y, x).val[2];
		}
	}
	//		cout<<maxdistH<<endl;
	//	cout<<maxdistS<<endl;
	//cout<<maxdistV<<endl;
}
Mat hsv;
Mat dst;
Mat colorMask;
int LH = 0; int HH = 86;
int LS = 0; int HS = 255;
int LV = 0; int HV = 255;

void TuneThreshold(int, void*){
	inRange(hsv, Scalar(LH, LS, LV), Scalar(HH, HS, HV), colorMask);

	img.copyTo(dst, colorMask);
	binary = colorMask;
	imshow("Tuning", binary);
}
void Tuning(){
	namedWindow("Tuning", 1);
	createTrackbar("LowH", "Tuning", &LH, 180, TuneThreshold);
	createTrackbar("HighH", "Tuning", &HH, 180, TuneThreshold);
	createTrackbar("LowS", "Tuning", &LS, 255, TuneThreshold);
	createTrackbar("HighS", "Tuning", &HS, 255, TuneThreshold);
	createTrackbar("LowV", "Tuning", &LV, 255, TuneThreshold);
	createTrackbar("HighV", "Tuning", &HV, 255, TuneThreshold);

	TuneThreshold(0, 0);
	waitKey(0);
}

void BackgroundRemove()
{	


	//Mat mask = Mat::zeros(img.rows, img.cols, CV_8U); 
	cvtColor(img, hsv, CV_BGR2HSV);
	
	
	if (tune){
		LH = 0; HH = 180;
		LS = 0; HS = 255;
		LV = 0; HV = 255;
		ColorDisplay();
		Tuning(); 
		imshow("result", dst);
	}
	else{
		cout << Scalar(LH, LS, LV) << endl << Scalar(HH, HS, HV)<<endl;
		inRange(hsv, Scalar(LH, LS, LV), Scalar(HH, HS, HV), colorMask);
		img.copyTo(dst, colorMask);
		binary = colorMask;
		if (binaryShow){
			ColorDisplay();
			imshow("Binary", binary);
			imshow("result", dst);
			waitKey();
			destroyAllWindows();
		}
	}
	
	if (save){
		imwrite("Mask.jpg", binary);
		imwrite("Extracted.jpg", dst);
	}

}

void Binarization(){
	Mat gray(img.size(), CV_8UC1);
	cvtColor(img, gray, CV_BGR2GRAY);
	
	imwrite("gray.jpg", gray);	
	cout << "Image binarizing..." << endl << endl;
	threshold(gray, binary, 250, 255, CV_THRESH_BINARY);
	if (binaryShow)
		imshow("Binarization", binary);
	imwrite("binary.jpg", binary);
	waitKey(0);
	destroyAllWindows();
}


void Refinement(){
	cout << "Image refining..." << endl << endl;
	if (refineShow){
		namedWindow("Refine Image", CV_WINDOW_AUTOSIZE);
		createTrackbar("Morph size:", "Refine Image", &morph_size, max_morph_size, RefineThreshold);
	}
	RefineThreshold(0, 0);
	waitKey(0);
	destroyAllWindows();
	if (save)
		imwrite("Refine.jpg", refine);
	
}
void RefineThreshold(int, void*){
	Mat element_close = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
	refine = binary.clone();

	morphologyEx(refine, refine, close_operator, element_close);
	if (refineShow)
		imshow("Refine Image", refine);
	
	//imwrite("refine.jpg", refine);
}