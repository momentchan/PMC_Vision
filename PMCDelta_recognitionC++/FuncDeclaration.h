#include "main.h"


class ObjectInfo{
public:
	ObjectInfo(){}
	void ObjectInfoAssign(vector<Point2f> centers, vector <double> orientations, vector <int> types, vector <int> colors){
		objectNum = centers.size();
		objectCenters = centers;
		objectOrientations = orientations;
		objectTypes = types;
		objectColors = colors;
	}
	void DisplayInfo(){
		cout << " Object Number : " << objectNum << endl;
		
		for (int i = 0; i < objectNum; i++)
			cout << " Object[" << i << "]" << " Type: " << objectTypes[i] << " Color: " << objectColors[i]
			<< " Center: " << objectCenters[i] << " Orientation: " << objectOrientations[i] << endl;
	}
	void ReadImage(){ img = imread("Picture.jpg"); }
	void ShowImage(){ imshow("",img); waitKey(0); }

private:
	Mat img;
	int objectNum;
	vector <Point2f> objectCenters;
	vector <double> objectOrientations;
	vector <int> objectTypes;
	vector <int> objectColors;
};



/** Function Headers */
void BackgroundRemove();
void Binarization();
void Refinement();
void RefineThreshold(int, void*);
void FindBlobs(const Mat &binary, vector < vector<Point2i> > &blobs, Mat &label_image);
void ConnectedComponent();
Mat HoleFilling(Mat);
void ObjectSegment();
void FindObjectCenter(vector <Mat>);
void CenterOrientation();
void OrientationExtraction();
void getOrientation(vector<Point> &, Mat&);
void RotateObject();
void WriteOutInfo();
string int2str(int &);
void saveImage(string, int, Mat);
void ColorDisplay();
static void onMouse(int event, int x, int y, int f, void*);
void HistogramCalulation(Mat, int&, int&, int&);
int findImageNum();
int Resgistration(Mat img1);
void Recognition();
int shapeDetection(Mat,int);
int colorDetection(Mat src);
void Translation();
void OutputImage();
void CopyData(int type[], int color[], double x[], double y[], double angle[]);