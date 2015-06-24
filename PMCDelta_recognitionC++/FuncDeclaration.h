#include "main.h"

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
void getOrientation(vector<Point> &, Mat&);
void RotateObject();
void WriteOutInfo();
string int2str(int &);
void saveImage(string, int, Mat);
void ColorDisplay();
static void onMouse(int event, int x, int y, int f, void*);