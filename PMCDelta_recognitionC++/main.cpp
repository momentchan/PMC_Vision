#include "main.h"
#include "FuncDeclaration.h"
#include <ctime>


//**********Variables for image binarization**********
Mat binary;
Mat refine;

//Variables for showing images
bool binaryShow = 0;
bool refineShow = 0;
bool separateShow = 0;
bool orientationShow = 0;
bool drawOrientation = 0;
bool showRotate = 0;
bool recogintionShow = 0;
bool convexShow = 0;
bool save = 0;
time_t now;
Mat img;
vector <Mat> separatedObjectsBlack;
vector <Mat> separatedObjectsColor;
vector <Mat> rotatedObjects;
vector <Point2f> objectCenters;
vector <double> objectOrientations;
vector <int> objectTypes;

int hour[2], mini[2], sec[2], milisec[2];

void setClock(int & hour, int & mini, int & sec, int & milisec){
	SYSTEMTIME st;
	GetSystemTime(&st);
	hour = st.wHour;
	mini = st.wMinute;
	sec = st.wSecond;
	milisec = st.wMilliseconds;
	//cout << hour << ":" << mini << ":" << sec << ":" << milisec << endl;
}

void CaptureFrame(){

	//setup video capture device and link it to the first capture device
	VideoCapture captureDevice;
	captureDevice.open(0);
	captureDevice.set(CV_CAP_PROP_FRAME_WIDTH, 1600);
	captureDevice.set(CV_CAP_PROP_FRAME_HEIGHT, 896);
	//while(1){
	captureDevice >> img;
	setClock(hour[0], mini[0], sec[0], milisec[0]);
	img = img(Rect(493, 70, 603, 663));
	//imshow("Detecting...",img);
	//cvWaitKey(33);
	//ColorDisplay();
	//cvWaitKey(0);
	//}//*/
	
	//cout << img.size();
	destroyAllWindows();
	if(save) imwrite("Picture.jpg", img);
	//system("cls");
}

void main()
{
	
	CaptureFrame();
	
	//img = imread("Picture.jpg");
	// Image Processing
	
	BackgroundRemove();
	///*
	//Binarization();
	Refinement();
	//refine = binary;
	ObjectSegment();
	CenterOrientation();
	RotateObject();
	Recognition();
	setClock(hour[1], mini[1], sec[1], milisec[1]);

	// Write out info
	WriteOutInfo();
	//*/

}

void WriteOutInfo(){
	ofstream outputFile;
	outputFile.open("ObjectInfo.txt");
	outputFile << hour[0] << ":" << mini[0] << ":" << sec[0] << ":" << milisec[0] << endl;
	outputFile << hour[1] << ":" << mini[1] << ":" << sec[1] << ":" << milisec[1] <<endl;
	for (int i = 0; i < objectCenters.size(); i++){
		outputFile << objectTypes[i] << " " << objectCenters[i] << " " << objectOrientations[i] << endl;
	}
	outputFile.close();

}
