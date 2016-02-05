#include "FuncDeclaration.h"

extern vector < vector<Point2i > > blobs;
Mat label_image;
extern Mat img;
extern Mat refine;
int minSize = 5000;
extern bool separateShow;
extern vector <Mat> separatedObjectsBlack;
extern vector <Mat> separatedObjectsColor;

void ObjectSegment(){
	//cout << "Separating objects..." << endl << endl;
	ConnectedComponent();
	for (int i = 0; i < blobs.size(); i++) {
	//for (vector < vector<Point2i > >::iterator  i =blobs.begin(); i != blobs.size(); i++) {
		Mat object = Mat::zeros(refine.size(), 16);
		int label = label_image.at<int>(blobs[i][0].y, blobs[i][0].x);
		/*if (blobs[i].size() < minSize){
			blobs.erase(blobs.begin() + i);
			i--;
			continue;
		}*/
		//cout << blobs[i].size() << endl;
		if (blobs[i].size() > minSize){
			for (int j = 0; j < blobs[i].size(); j++) {
				int x = blobs[i][j].x;
				int y = blobs[i][j].y;
				object.at<Vec3b>(y, x) = Vec3b(255, 255, 255);
			}
			object = HoleFilling(object);
			separatedObjectsBlack.push_back(object);
		}
		//if (separateShow){
		//	imshow("Separated Objects", separatedObjectsBlack[i]);
		//	waitKey(0);
		//}
	}

	// Recover original color
	for (int i = 0; i < separatedObjectsBlack.size(); i++){
		Mat object = separatedObjectsBlack[i].clone();

		for (int x = 0; x < separatedObjectsBlack[i].cols; x++)
		for (int y = 0; y < separatedObjectsBlack[i].rows; y++){
			if (separatedObjectsBlack[i].at<Vec3b>(y, x) == Vec3b(0, 0, 0)){

				Vec3b color = img.at<Vec3b>(y, x);
				object.at<Vec3b>(y, x) = color;
			}
			else{
				object.at<Vec3b>(y, x) = Vec3b(255, 255, 255);
			}
		}
		separatedObjectsColor.push_back(object);
		if (separateShow){
			imshow("Separated Objects", object);
			waitKey(0);
		}
	}
	destroyAllWindows();

}
void ConnectedComponent(){
	Mat invert;
	bitwise_not(refine, invert);
	threshold(invert, invert, 0.0, 1.0, THRESH_BINARY);
	FindBlobs(invert, blobs, label_image);
}
void FindBlobs(const Mat &binary, vector < vector<Point2i> > &blobs, Mat &label_image)
{
	blobs.clear();

	// Fill the label_image with the blobs
	// 0  - background
	// 1  - unlabelled foreground
	// 2+ - labelled foreground

	binary.convertTo(label_image, CV_32SC1);

	int label_count = 2; // starts at 2 because 0,1 are used already

	for (int y = 0; y < label_image.rows; y++) {
		int *row = (int*)label_image.ptr(y);
		for (int x = 0; x < label_image.cols; x++) {
			if (row[x] != 1) {
				continue;
			}

			Rect rect;
			floodFill(label_image, Point(x, y), label_count, &rect, 0, 0, 4);

			vector <Point2i> blob;

			for (int i = rect.y; i < (rect.y + rect.height); i++) {
				int *row2 = (int*)label_image.ptr(i);
				for (int j = rect.x; j < (rect.x + rect.width); j++) {
					if (row2[j] != label_count) {
						continue;
					}
					blob.push_back(Point2i(j, i));
				}
			}

			blobs.push_back(blob);

			label_count++;
		}
	}
}