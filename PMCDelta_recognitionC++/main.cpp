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
bool save = 1;
bool output = 1;

time_t now;
Mat img;
vector <Mat> separatedObjectsBlack;
vector <Mat> separatedObjectsColor;
vector <Mat> rotatedObjects;
vector <Point2f> objectCenters;
vector <Mat> originObjects;
vector <double> objectOrientations;
vector <int> objectTypes;
vector <int> objectColors;
vector < vector<Point2i > > blobs;

//int hour[2], mini[2], sec[2], milisec[2];
int startTime[4];
void setClock(int time []){

	SYSTEMTIME st;
	GetLocalTime(&st);
	time[0] = st.wHour;
	time[1] = st.wMinute;
	time[2] = st.wSecond;
	time[3] = st.wMilliseconds;
	
	//cout << hour << ":" << mini << ":" << sec << ":" << milisec << endl;
}
int corner_x=480, corner_y=40, w=594, h=700;
void DrawRectangle(int, void*){
	rectangle(img, Rect(corner_x, corner_y, w, h), (255, 0, 0), 2);
}
void CaptureFrame(){

	//setup video capture device and link it to the first capture device
	VideoCapture captureDevice(0);
	if (!captureDevice.isOpened()) { //check if video device has been initialised
		cout << "cannot open camera";
	}
	captureDevice.open(0);
	captureDevice.set(CV_CAP_PROP_FRAME_WIDTH, 1600);
	captureDevice.set(CV_CAP_PROP_FRAME_HEIGHT, 896);
	while(video){
		captureDevice >> img;
		//setClock(hour[0], mini[0], sec[0], milisec[0]);
		//img = img(Rect(500, 89, 600, 690));
		namedWindow("Rectangle");
		createTrackbar("X", "Rectangle", &corner_x, 520, DrawRectangle);
		createTrackbar("Y", "Rectangle", &corner_y, 100, DrawRectangle);
		createTrackbar("W", "Rectangle", &w, 620, DrawRectangle);
		createTrackbar("H", "Rectangle", &h, 700, DrawRectangle);
		DrawRectangle(0, 0);
		imshow("Detecting...", img);
		if (waitKey(30) == 27)
			break;
	}
	captureDevice >> img;
	setClock(startTime);
	//setClock(hour[0], mini[0], sec[0], milisec[0]);
	img = img(Rect(corner_x, corner_y, w, h));
	imwrite("Picture.jpg", img);
	
	//cout << img.size();
	destroyAllWindows();
	//if(save)
	//system("cls");
}
void ClearAll(){
	separatedObjectsBlack.clear();
	separatedObjectsColor.clear();
	rotatedObjects.clear();
	objectCenters.clear();
	objectOrientations.clear();
	objectTypes.clear();
	objectColors.clear();
	blobs.clear();
	originObjects.clear();
}
DLL int main(int type[], int color[], double x[], double y[], double angle[])
//int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show)
{
	//int type[10];
	//ObjectInfo *objectinfo = new ObjectInfo();
	//objectinfo->ReadImage();
	//objectinfo->ShowImage();
	///*
	if (video||tune)
		CaptureFrame();
	//imshow("", img); waitKey(0.33);
	//cout << endl<<"Object Dectecting....."<<endl;
	setClock(startTime);
	if (!tune)
	{
		img = imread("Capture.jpg");
		//imshow("", img); waitKey(0);
		img = img(Rect(corner_x, corner_y, w, h));
	}
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
		//Old version

		OrientationExtraction();
		//setClock(hour[1], mini[1], sec[1], milisec[1]);
		WriteOutInfo();
		if (output)
			CopyData(type, color, x, y, angle);
		int num = objectTypes.size();
		ClearAll();
		return num;
		
		//cout << x[0] << endl;
		// Write out info
	}
	
	
}

void OutputImage(){
	//Point2f src_center(img.cols / 2.0F, img.rows / 2.0F);
	//Mat rot_mat = getRotationMatrix2D(src_center, 90,1);
	//Mat dst;
	//warpAffine(img, dst, rot_mat, Size(img.rows,img.cols));
	//Mat dst;
	//transpose(img, dst);
	//flip(dst, dst, -1);
	//flip(dst, dst, 1);
	imwrite("Result.jpg", img);
}
void WriteOutInfo(){
	ofstream outputFile;
	outputFile.open("ObjectInfo.txt");
	// Txt format:
	// Start Time
	// End Time
	// Type Color Center Orientation

	outputFile << startTime[0] << ":" << startTime[1] << ":" << startTime[2] << ":" << startTime[3] << endl;
	//outputFile << hour[1] << ":" << mini[1] << ":" << sec[1] << ":" << milisec[1] <<endl;
	for (int i = 0; i < objectCenters.size(); i++){
		outputFile << objectTypes[i] << " " << objectColors[i] << " " << objectCenters[i]  << " " << objectOrientations[i] << endl;
	}
	outputFile.close();
}
void CopyData(int type[], int color[], double x[], double y[], double angle[]){
	for (int i = 0; i < objectTypes.size(); i++){
		type[i] = objectTypes[i];
		color[i] = objectColors[i];
		x[i] = objectCenters[i].x*10;
		y[i] = objectCenters[i].y*10;
		angle[i] = objectOrientations[i];
	}
	//for (int i = 0; i < 4; i++)
		//time[i] = startTime[i];

}