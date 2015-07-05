#include "FuncDeclaration.h"
#include <ctime>
#define DLL extern "C" __declspec(dllexport)


//**********Variables for image binarization**********
Mat binary;
Mat refine;

//Variables for showing images
bool video = 0;
bool tune = 0;
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
vector <int> objectColors;

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
	while(video){
		captureDevice >> img;
		//setClock(hour[0], mini[0], sec[0], milisec[0]);
		//img = img(Rect(493, 70, 603, 663));
		imshow("Detecting...",img);
		cvWaitKey(33);
		imwrite("Picture.jpg", img);

	}
	captureDevice >> img;
	setClock(hour[0], mini[0], sec[0], milisec[0]);
	img = img(Rect(480, 35, 593, 704));
	
	
	//cout << img.size();
	destroyAllWindows();
	if(save) imwrite("Picture.jpg", img);
	//system("cls");
}

//DLL void main()
int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show)
{
	
	//ObjectInfo *objectinfo = new ObjectInfo();
	//objectinfo->ReadImage();
	//objectinfo->ShowImage();
	///*
	//CaptureFrame();  
    img = imread("Picture.jpg");

	// Image Processing
	
	BackgroundRemove();
	if (!tune){
	//Binarization();
	Refinement();
	//refine = binary;
	ObjectSegment();
	CenterOrientation();
	OutputImage();
	RotateObject();
	Translation();
	Recognition();
	setClock(hour[1], mini[1], sec[1], milisec[1]);

	// Write out info
	WriteOutInfo();
	
	}
	
}

void OutputImage(){
	//Point2f src_center(img.cols / 2.0F, img.rows / 2.0F);
	//Mat rot_mat = getRotationMatrix2D(src_center, 90,1);
	//Mat dst;
	//warpAffine(img, dst, rot_mat, Size(img.rows,img.cols));
	Mat dst;
	transpose(img, dst);
	flip(dst, dst, -1);
	imwrite("Result.jpg", dst);
}
void WriteOutInfo(){
	ofstream outputFile;
	outputFile.open("ObjectInfo.txt");
	// Txt format:
	// Start Time
	// End Time
	// Type Color Center Orientation

	outputFile << hour[0] << ":" << mini[0] << ":" << sec[0] << ":" << milisec[0] << endl;
	outputFile << hour[1] << ":" << mini[1] << ":" << sec[1] << ":" << milisec[1] <<endl;
	for (int i = 0; i < objectCenters.size(); i++){
		outputFile << objectTypes[i] << " " << objectColors[i] << " " << objectCenters[i]  << " " << objectOrientations[i] << endl;
	}
	outputFile.close();

}
