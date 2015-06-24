#include "main.h"
#include "FuncDeclaration.h"
#include <ctime>


//**********Variables for image binarization**********
Mat binary;
Mat refine;

//Variables for showing images
bool binaryShow = 0;
bool refineShow = 1;
bool separateShow = 1;
bool orientationShow = 1;
bool drawOrientation = 1;
bool showRotate = 1;
time_t now;
Mat img;
vector <Mat> separatedObjectsBlack;
vector <Mat> separatedObjectsColor;
vector <Mat> rotatedObjects;
vector <Point2f> objectCenters;
vector <double> objectOrientations;
void CaptureFrame(){

	//setup video capture device and link it to the first capture device
	VideoCapture captureDevice;
	captureDevice.open(0);
	captureDevice.set(CV_CAP_PROP_FRAME_WIDTH, 1600);
	captureDevice.set(CV_CAP_PROP_FRAME_HEIGHT, 896);
	//while(1){
	captureDevice >> img;
	now = time(0);
	img = img(Rect(660, 220, 300, 300));
	//imshow("Detecting...",img);
	//cvWaitKey(33);
	//cvWaitKey(0);
	//}
	cout << img.size();
	destroyAllWindows();
	//imwrite("Picture.jpg", img);
	system("cls");
}

void main()
{

	//CaptureFrame();
	
	img = imread("Picture.jpg");
	// Image Processing
	BackgroundRemove();
	
	//Binarization();
	Refinement();
	ObjectSegment();
	CenterOrientation();
	RotateObject();
	// Write out info
	WriteOutInfo();

}

void WriteOutInfo(){
	ofstream outputFile;
	outputFile.open("ObjectInfo.txt");
	tm *startTime = localtime(&now);
	outputFile<<"Time:	"<< startTime->tm_hour<<":"<<startTime->tm_min<<":"<<startTime->tm_sec<< endl;
	outputFile << "Id	Type	Center		Orientation" << endl;
	for (int i = 0; i < objectCenters.size(); i++){
		outputFile << int2str(i) << "	0	" << objectCenters[i] <<"	"<< objectOrientations[i]/3.14*180 << endl;
	}
	outputFile.close();

}
