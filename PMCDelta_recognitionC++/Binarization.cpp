#include "FuncDeclaration.h"

extern bool binaryShow;
extern bool refineShow;
extern bool save;
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

void BackgroundRemove()
{	
	Mat hsv; 
	Mat dst;
	Mat colorMask; 

	//Mat mask = Mat::zeros(img.rows, img.cols, CV_8U); 
	cvtColor(img, hsv, CV_BGR2HSV);

	/*HistogramCalulation(hsv, meanH, meanS, meanV);

	cout<<"Major Color: "<<meanH<<" "<<meanS<<" "<<meanV<<endl;

	int rangeH[2]; rangeH[0] = 180; rangeH[1] = 0;
	int rangeS[2]; rangeS[0] = 255; rangeS[1] = 0;
	int rangeV[2]; rangeV[0] = 255; rangeV[1] = 0;
	findRange(hsv, meanH, meanS, meanV, rangeH, rangeS, rangeV);

	cout<<"Color Range: "<<endl;
	cout<<rangeH[0]<<" "<<rangeH[1]<<endl;
	cout<<rangeS[0]<<" "<<rangeS[1]<<endl;
	cout<<rangeV[0]<<" "<<rangeV[1]<<endl;
	*/

	//inRange(hsv, Scalar(rangeH[0], rangeS[0], rangeV[0]), Scalar(rangeH[1]+5, 255, rangeV[1]+5), colorMask);
	inRange(hsv, Scalar(0, 0, 0), Scalar(255, 150, 80), colorMask);
	
	img.copyTo(dst, colorMask);
	binary = ~colorMask;
	if (binaryShow){
		ColorDisplay();
		imshow("mask", binary);//show mask
		imshow("result", dst);//showµ²ªG
		waitKey();
		destroyAllWindows();
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