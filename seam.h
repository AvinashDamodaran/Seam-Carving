#ifndef SEAM_H
#define SEAM_H

#include "mainwindow.h"
#include <set>

#define HOR 1
#define VER 2
#define coord pair<int,int>		// order is (row,col)

#define RED Scalar(0,0,255)
#define BLOOD_RED Scalar(7,7,138)
#define BLUE Scalar(255,0,0)
#define GREEN Scalar(0,255,0)
#define WHITE Scalar(255,255,255)
#define BLACK Scalar(0,0,0)
#define GRAY Scalar(230,230,230)

using namespace std;
using namespace cv;


struct node{
	int path;	// left is -1, middle is 0, right is 1
	long long int energy;
};

struct by_energy { 
	bool operator()(node const &a, node const &b) {
		return a.energy < b.energy;
	}
};

Mat seamNormal(Mat img, int cols_to_delete, int mode = VER, bool show = true);
Mat viewPyramids(vector<Mat> pyramids);
Mat pyUp(Mat img);
vector<Mat> GPyramids(Mat img, int num_levels);
vector<Mat> LPyramids(vector<Mat> gaussianPyramids);
pair< Mat , vector< vector< coord > > > seam(Mat img, int cols_to_delete);
Mat removeSeams(Mat img, vector< vector< coord > > seams);
vector< vector< coord > > mapSeams(vector< vector< coord > > seams);
Mat seamPyramid(vector<Mat> &gaussians, int cols_to_delete);



struct pointComp {
	bool operator() (const Point& lhs, const Point& rhs) const
	{
		if (lhs.x<rhs.x) return true;
		else if (lhs.x>rhs.x) return false;
		else {
			return lhs.y<rhs.y;
		}
	}
};

extern Mat obj_img;
extern set<Point,pointComp> save;
extern set<Point,pointComp> rem;
extern string draw_winname;

void obj_draw(int event, int x, int y, int flags, void* param);

#endif
