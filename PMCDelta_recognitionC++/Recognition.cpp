#include "FuncDeclaration.h"

extern vector <Mat> separatedObjectsBlack;
extern vector <Mat> rotatedObjects;
extern vector <int> objectTypes;
extern vector <int> objectColors;
extern bool save;
extern Mat binary;
extern bool recogintionShow;
extern bool convexShow;

void Recognition(){
	// Type Recogition
	for (int i = 0; i < separatedObjectsBlack.size(); i++){
		objectTypes.push_back(shapeDetection(rotatedObjects[i]));
		objectColors.push_back(colorDetection(rotatedObjects[i]));
	}

}
int colorDetection(Mat src){
	Mat hsv;
	cvtColor(src, hsv, CV_BGR2HSV);
	int H = 0, S = 0, V= 0;
	int num = 0;
	for (int i = 0; i < hsv.rows; i++)
		for (int j = 0; j < hsv.cols;j++){
			if (src.at<Vec3b>(i, j) != Vec3b(255, 255, 255)){
				//cout << src.at<Vec3b>(i, j) << endl;// .val[0] << endl;
				H += src.at<Vec3b>(i, j).val[0];
				S += src.at<Vec3b>(i, j).val[1];
				V += src.at<Vec3b>(i, j).val[2];
				num++;
			}
	}
	H /= num;
	S /= num;
	V /= num;
	Vec3b color = Vec3b(H, S, V);
	
	
	
	//cout << norm(color) << " " << norm(Vec3b(255, 255, 255) - color)<< endl;

	return norm(color)>norm(Vec3b(255, 255, 255) - color)? 1:0;
}

static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void setLabel(Mat& im, const std::string label, std::vector<Point>& contour)
{
	int fontface = FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	Size text = getTextSize(label, fontface, scale, thickness, &baseline);
	Rect r = boundingRect(contour);

	Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	rectangle(im, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}

vector<Point> convexHullExtraction(Mat src){
	Mat src_gray;
	cvtColor(src, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));

	// Convex Hull implementation
	Mat src_copy = src.clone();
	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// Find contours
	threshold(src_gray, threshold_output, 200, 255, THRESH_BINARY);
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Find the convex hull object for each contour
	vector<vector<Point> >hull(1);
	convexHull(Mat(contours[1]), hull[0], false);


	// Draw contours + hull results
	if (convexShow){
		RNG rng;
		Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
		for (int i = 0; i< contours.size(); i++)
		{
			if (i == 1)
				drawContours(drawing, contours, i, Scalar(255, 255, 0), 1, 8, vector<Vec4i>(), 0, Point());
			if (i == 0)
				drawContours(drawing, hull, i, Scalar(255, 255, 255), 1, 8, vector<Vec4i>(), 0, Point());
		}
		// Show in a window
		namedWindow("Hull demo", CV_WINDOW_AUTOSIZE);
		imshow("Hull demo", drawing);
		if (save)
			imwrite("Hull.jpg", drawing);
		waitKey(0);
	}

	return hull[0];
}


int shapeDetection(Mat src){
	
	// Do convex hull refinement
	vector<Point> hull = convexHullExtraction(src);

	///*
	std::vector<Point> approx;
	Mat dst = src.clone();
	int shape = -1;

	// Approximate contour with accuracy proportional to the contour perimeter
	approxPolyDP(Mat(hull), approx, arcLength(Mat(hull), true)*0.03, true);

	//cout << approx.size() << endl;
	if (approx.size() == 3){
		setLabel(dst, "TRI", hull);    // Triangles
		shape = 3;
	}
	else if (approx.size() >= 4 && approx.size() <= 6){
		// Number of vertices of polygonal curve
		int vtc = approx.size();

		// Get the cosines of all corners
		std::vector<double> cos;
		for (int j = 2; j < vtc + 1; j++)
		cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

		// Sort ascending the cosine values
		std::sort(cos.begin(), cos.end());

		// Get the lowest and the highest cosine
		double mincos = cos.front();
		double maxcos = cos.back();

		// Use the degrees obtained above and the number of vertices
		// to determine the shape of the contour
		if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3){
			setLabel(dst, "RECT", hull); 
			shape = 4;
		}
		else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27){
			setLabel(dst, "PENTA", hull); 
			shape = 5;
		}
		else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45){
			setLabel(dst, "HEXA", hull); 
			shape = 6;
		}
	}
	else{
		// Detect and label circles
		double area = contourArea(hull);
		Rect r = boundingRect(hull);
		int radius = r.width / 2;

		if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
		std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
		setLabel(dst, "CIR", hull);
		shape = 0;
	}
	if (recogintionShow){
		//imshow("src", src);
		imshow("dst", dst);
		if (save)
			imwrite("Result.jpg", dst);
		waitKey(0);
	}
	return shape;
	//*/
}