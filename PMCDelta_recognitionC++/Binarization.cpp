#include "FuncDeclaration.h"

extern bool binaryShow;
extern bool refineShow;
extern Mat img;
extern Mat binary;
extern Mat refine;

int morph_elem = 0;
int morph_size = 3;
int const max_morph_size = 10;
int const open_operator = 2;
int const close_operator = 3;


void BackgroundRemove()
{

	Mat hsv; //轉到hsv平面
	Mat dst; //結果圖片
	Mat colorMask; //各顏色的閥值

	//Mat mask = Mat::zeros(img.rows, img.cols, CV_8U); //為了濾掉其他顏色
	cvtColor(img, hsv, CV_BGR2HSV);//轉成hsv平面
	
	inRange(hsv, Scalar(50, 50, 60), Scalar(90, 255, 255), colorMask);
	img.copyTo(dst, colorMask); //將原圖片經由遮罩過濾後，得到結果dst
	binary = colorMask;
	if (binaryShow){
		imshow("mask", colorMask);//show mask
		imshow("img", img);//show原圖片
		imshow("result", dst);//show結果
		ColorDisplay();
		waitKey();
		destroyAllWindows();
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
}
void RefineThreshold(int, void*){
	Mat element_close = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
	refine = binary.clone();

	morphologyEx(refine, refine, close_operator, element_close);
	if (refineShow)
		imshow("Refine Image", refine);
	//imwrite("refine.jpg", refine);
}