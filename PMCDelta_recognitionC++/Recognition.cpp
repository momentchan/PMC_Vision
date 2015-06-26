#include "FuncDeclaration.h"

extern vector <Mat> separatedObjectsBlack;
extern vector <Mat> rotatedObjects;
extern vector <int> objectType;
extern Mat binary;
void Recognition(){

	for (int i = 0; i < separatedObjectsBlack.size(); i++){
		shapeDetection(rotatedObjects[i]);
	
		//objectType.push_back(Resgistration(img));
		//cout << objectType[i]<<endl;
	}
}

int Resgistration(Mat img1){

	Mat gray;
	cvtColor(img1, gray, CV_BGR2GRAY);



	return 0;


	/*
	int T = (double)(img1.rows * img1.cols) * 0.7;
	threshold(img1, img1, 128, 255, CV_THRESH_BINARY_INV);
	imshow("", img1); waitKey(0);
	int maxMatch = 0;
	int maxIndex = 0;
	Mat res;

	for (int i = 0; i < 3; i++){
	string file = "Data/Object";
	file.append(int2str(i));
	file.append(".jpg");
	Mat img2 = imread(file,16);
	threshold(img2, img2, 128, 255, CV_THRESH_BINARY_INV);

	bitwise_and(img1, img2, res);
	int num = countNonZero(res);
	cout << num << endl;
	imshow("AND", res); waitKey(0);


	if (num > maxMatch){
	maxMatch = num;
	maxIndex = i;
	}
	}
	return maxIndex;*/
}

static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/**
* Helper function to display text in the center of a contour
*/
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

void shapeDetection(Mat src){
	
	Mat src_gray;
	cvtColor(src, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));
	namedWindow("Source", CV_WINDOW_AUTOSIZE);
	imshow("Source", src);

	// Convex Hull implementation
	Mat src_copy = src.clone();
	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// Find contours
	threshold(src_gray, threshold_output, 200, 255, THRESH_BINARY);
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Find the convex hull object for each contour
	vector<vector<Point> >hull(contours.size());
	for (int i = 0; i < contours.size(); i++)
		convexHull(Mat(contours[i]), hull[i], false);

	
	// Draw contours + hull results
	RNG rng;
	Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		drawContours(drawing, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	}
	
	// Show in a window
	namedWindow("Hull demo", CV_WINDOW_AUTOSIZE);
	imshow("Hull demo", drawing);

	waitKey(0);
	
	
	//*/

	std::vector<Point> approx;
	Mat dst = src.clone();
	contours = hull;
	for (int i = 0; i < contours.size(); i++)
	{
	// Approximate contour with accuracy proportional
	// to the contour perimeter
		//cout<<ar
	approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.002, true);

	// Skip small or non-convex objects
	if (std::fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
	continue;
	cout << approx.size() << endl;
	if (approx.size() == 3)
		setLabel(dst, "TRI", contours[i]);    // Triangles
	else if (approx.size() >= 4 && approx.size() <= 6)
	{
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
		if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3)
		setLabel(dst, "RECT", contours[i]);
		else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27)
		setLabel(dst, "PENTA", contours[i]);
		else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45)
		setLabel(dst, "HEXA", contours[i]);
	}
	else
	{
		// Detect and label circles
		double area = contourArea(contours[i]);
		Rect r = boundingRect(contours[i]);
		int radius = r.width / 2;

		if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
		std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
		setLabel(dst, "CIR", contours[i]);
		}
	}

	imshow("src", src);
	imshow("dst", dst);
	waitKey(0);
	//*/
}