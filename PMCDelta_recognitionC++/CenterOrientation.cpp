#include "FuncDeclaration.h"

extern vector <Mat> separatedObjectsBlack;
extern vector <Mat> separatedObjectsColor;
extern vector <Mat> rotatedObjects;
extern vector <Point2f> objectCenters;
extern vector <double> objectOrientations;

extern bool drawOrientation;
extern bool orientationShow;
extern bool showRotate;

extern Mat img;

Point2f displace(-17.8, 41.5);
Point2f imageCenter(593 / 2, 704 / 2);
float scaleRatio = 0.036;  // 25/704 cm
int windowSize = 300;


void CenterOrientation(){
	cout << "Computing center and orientation..." << endl << endl;
	//cout << separatedObjectsBlack.size();
	for (int i = 0; i < separatedObjectsBlack.size(); i++){
		Mat gray;
		cvtColor(separatedObjectsBlack[i], gray, COLOR_BGR2GRAY);
		
		// Apply thresholding;
		threshold(gray, gray, 150, 255, CV_THRESH_BINARY);
		
		// Find all the contours in the thresholded image
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(gray, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

		for (size_t j = 0; j < 1; ++j)
		{
			// Calculate the area of each contour
			double area = contourArea(contours[j]);
			// Ignore contours that are too small or too large
			if (area < 1e2 || 1e5 < area) {
				separatedObjectsBlack.erase(separatedObjectsBlack.begin() + i);
				separatedObjectsColor.erase(separatedObjectsColor.begin() + i);
				i--;
				continue;
			}
			drawContours(img, contours, 0, Scalar(0, 0, 255), 2);
			// Find the orientation of each shape
			if (drawOrientation){
				getOrientation(contours[j], separatedObjectsColor[i]);
			}
			else {
				Mat src = separatedObjectsColor[i].clone();
				getOrientation(contours[j], src);
			}
		}
	}
	destroyAllWindows();
}
void getOrientation(vector<Point> &pts, Mat &src)
{
	//Construct a buffer used by the pca analysis
	Mat data_pts = Mat(pts.size(), 2, CV_64FC1);
	for (int i = 0; i < data_pts.rows; ++i) {
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
	}
	//Perform PCA analysis
	PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);

	//Store the position of the object
	Point pos = Point2f(pca_analysis.mean.at<double>(0, 0), pca_analysis.mean.at<double>(0, 1));

	//Store the eigenvalues and eigenvectors
	vector<Point2d> eigen_vecs(2);
	vector<double> eigen_val(2);
	for (int i = 0; i < 2; ++i) {
		eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
			pca_analysis.eigenvectors.at<double>(i, 1));
		//eigen_val[i] = pca_analysis.eigenvalues.at<double>(0, i);
	}
	eigen_val[0] = pca_analysis.eigenvalues.at<double>(0, 0);
	eigen_val[1] = pca_analysis.eigenvalues.at<double>(0, 0);

	if (drawOrientation){
		// Draw the principal components
		circle(src, pos, 10, CV_RGB(255, 255, 255), 2);
		line(src, pos, pos + 0.02 * Point(eigen_vecs[0].x * eigen_val[0], eigen_vecs[0].y * eigen_val[0]), CV_RGB(255, 255, 0));
		line(src, pos, pos + 0.02 * Point(eigen_vecs[1].x * eigen_val[1], eigen_vecs[1].y * eigen_val[1]), CV_RGB(0, 255, 255));
	}
	circle(img, pos, 10, CV_RGB(255, 255, 255), 2);
	line(img, pos, pos + 0.02 * Point(eigen_vecs[0].x * eigen_val[0], eigen_vecs[0].y * eigen_val[0]), CV_RGB(255, 255, 0));
	line(img, pos, pos + 0.02 * Point(eigen_vecs[1].x * eigen_val[1], eigen_vecs[1].y * eigen_val[1]), CV_RGB(0, 255, 255));

	if (orientationShow){
		imshow("Object Orientation", src);
		waitKey(0);
	}
	
	objectCenters.push_back(pos);
	objectOrientations.push_back(atan2(eigen_vecs[0].y, eigen_vecs[0].x) / 3.14 * 180);
}
void RotateObject()
{

	for (int i = 0; i < separatedObjectsBlack.size(); i++){
		Point2f leftTop = objectCenters[i] - Point2f(windowSize / 2, windowSize / 2);
		// Extract objects and store in a windowSize x windowSize matrix
		//Mat src = separatedObjectsColor[i];

		Mat src = Mat::zeros(Size(windowSize, windowSize), 16);
		for (int x = leftTop.x; x < (leftTop.x + windowSize); x++)
		for (int y = leftTop.y; y < (leftTop.y + windowSize); y++){
			if (x<0 || x >= separatedObjectsBlack[i].cols || y<0 || y >= separatedObjectsBlack[i].rows)
				src.at<Vec3b>(y - leftTop.y, x - leftTop.x) = Vec3b(255, 255, 255);
			else{
				if (separatedObjectsBlack[i].at<Vec3b>(y, x) == Vec3b(0, 0, 0))
					src.at<Vec3b>(y - leftTop.y, x - leftTop.x) = separatedObjectsColor[i].at<Vec3b>(y, x);
				else
					src.at<Vec3b>(y - leftTop.y, x - leftTop.x) = Vec3b(255, 255, 255);
			}
		}



		int len = max(src.cols, src.rows);
		//Point2f center(len / 2., len / 2.);
		Point2f center = Point2f(windowSize / 2, windowSize / 2);
		double angle = objectOrientations[i] * 180 / 3.14 - 90;
		Mat rot = getRotationMatrix2D(center, angle, 1.0);

		Mat dst;
		warpAffine(src, dst, rot, Size(len, len), INTER_CUBIC, BORDER_CONSTANT, Scalar(255, 255, 255));
		rotatedObjects.push_back(dst);
		if (showRotate){
			imshow("rotateObject", src);
			waitKey(0);
			imshow("rotateObject", dst);
			waitKey(0);
		}
		saveImage("Original Objects/Object", i, src);
		saveImage("Rotated Objects/Object", i, dst);
	}
}
void Translation(){

	for (int i = 0; i < objectCenters.size(); i++){
		objectCenters[i] = (objectCenters[i] - Point2f(separatedObjectsBlack[i].cols / 2, separatedObjectsBlack[i].rows / 2))* scaleRatio + displace;
	}
}