#pragma once
#include "imageIO/MyImage.h"
#include "imageIO/ImageFrameWndManager.h"
#include "Features.h"
#include <vector>
#include <math.h>

using namespace std;
struct PointS {// snake 위의 한점
	int x;
	int y;
};

typedef vector<PointS>	VECTOR_P;
typedef vector<int>		VECTOR_int;
typedef vector<double>	VECTOR_Dounle;

class Snake {

public:
	// *************************  생성자 및 소멸자  ************************
	Snake(void);
	Snake(int width, int height, int*  gradient, int*  flow, VECTOR_P circle);
	~Snake(void);

	//*****************************  멤버 변수  *****************************
	
	// 1. Points of the snake
	VECTOR_P snake;		//VECTOR_P circle;
	
	// 3. Length of the snake (euclidean distance)
	int snakelength;

	// 2. size of the image (and of the 2 arrays below)
	int width;
	int height;
	int imageSize;


	// 4. gradient value & flow (modulus)
	int* chanel_gradient;
	int* chanel_flow;

	// 5. 3x3 neighborhood used to compute energies
	double e_uniformity[3][3];
	double e_curvature[3][3];
	double e_flow[3][3];
	double e_inertia[3][3];

	// 6. coefficients for the 4 energy functions
	double alpha;	// alpha = coefficient for uniformity(high = > force equals distance between points)	// internal energy: elasticity energy
	double beta;	// beta  = coefficient for curvature  (high => force smooth curvature)					// internal energy: curvature energy		
	double gamma;	// gamma  = coefficient for flow      (high => force gradient attraction)				// external energy: gradient flow
	double delta;	// delta  = coefficient for intertia  (high => get stuck to gradient)

	// 7. according to distance between points
	bool AUTOADAPT;
	int	 AUTOADAPT_LOOP;
	int  AUTOADAPT_MINLEN;
	int  AUTOADAPT_MAXLEN;

	// 8. maximum number of iterations (if no convergence)
	int MAXITERATION;

public:
	// **************************  멤버함수 1 : START SNAKE SEGMENTATION   *********************
	int loop();				// 1. Snake 반복 수행 
	bool step();			// 2. update the position of each point of the snake

	void normalize(double array3x3[3][3]);	// normalize energy matrix
	double getsnakelength();				// total length of snake
	double distance2D(PointS a, PointS b);

	// **************************  멤버함수 2 : ENERGY FUNCTIONS   *********************
	double f_uniformity(PointS prev, PointS next, PointS p);
	double f_curvature(PointS prev, PointS p, PointS next);
	double f_gflow(PointS cur, PointS p);
	double f_inertia(PointS cur, PointS p);
	
	// **************************  멤버함수 3 : AUTOADAPT   *****************************
	void rebuild(int space);				// rebuild the snake using cubic spline interpolation
	void removeOverlappingPoints(int minlen);
	void addMissingPoints(int maxlen);
};


// ******************  SNAKE 수행을 위한 일반 함수(Snake class 함수 아님)  *****************************
// 1. 
void computegflow(const CByteImage& imageIn, int* chanel_gradient, int* chanel_flow, int THRESHOLD);

// 2. 
VECTOR_P initialPoint(int in_MAXLEN);

// 5. 
void display(CByteImage& m_imageOut, VECTOR_P snake);




class ChamferDistance {
public:


	int* chamfer;
	int normalizer;
	int width = 0, height = 0;
	int chamfer_length = 3;


	// 생성자
	ChamferDistance() {
	}

	ChamferDistance(int *chamfer5) {
		chamfer = chamfer5;
		normalizer = chamfer5[2];
	}

	void testAndSet(double* output, int x, int y, double newvalue) {
		if (x<0 || x >= width) return;
		if (y<0 || y >= height) return;
		double v = output[y*width + x];
		if (v >= 0 && v<newvalue) return;
		output[y*width + x] = newvalue;
	}

	void compute(double * cdist, bool* input, int width, int height) {
		int dx = 0; int dy = 0; int dt = 0;
		width = width;
		height = height;
		//double* output = new double[width*height];

		// initialize distance
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (input[y*width + x])
					cdist[y*width + x] = 0; // inside the object -> distance=0
				else
					cdist[y*width + x] = -1; // outside the object -> to be computed
			}
		}

		 // forward
		for (int y = 0; y <= height - 1; y++) {
			for (int x = 0; x <= width - 1; x++) {


				double v = cdist[y*width + x];
				if (v<0) continue;	// outside the object 일때만 수행하면


				for (int k = 0; k<chamfer_length; k++) {
					dx = chamfer[0*3 + k];
					dy = chamfer[1*3 + k];
					dt = chamfer[2*3 + k];

							testAndSet(cdist, x + dx, y + dy, v + dt);
					if (dy != 0) { 
							testAndSet(cdist, x - dx, y + dy, v + dt); 
					}
					if (dx != dy) {	
							testAndSet(cdist, x + dy, y + dx, v + dt);
						if (dy != 0) { 
							testAndSet(cdist, x - dy, y + dx, v + dt); 
						}
					}
				}



			}
		}

		// backward
		for (int y = height - 1; y >= 0; y--) {
			for (int x = width - 1; x >= 0; x--) {
				double v = cdist[y*width + x];
				if (v<0) continue;
				for (int k = 0; k<chamfer_length; k++) {
					dx = chamfer[0 * 3 + k];
					dy = chamfer[1 * 3 + k];
					dt = chamfer[2 * 3 + k];

					testAndSet(cdist, x - dx, y - dy, v + dt);
					if (dy != 0) testAndSet(cdist, x + dx, y - dy, v + dt);
					if (dx != dy) {
						testAndSet(cdist, x - dy, y - dx, v + dt);
						if (dy != 0) testAndSet(cdist, x + dy, y - dx, v + dt);
					}
				}
			}
		}

		// normalize
		for (int y = 0; y<height; y++)
			for (int x = 0; x<width; x++)
				cdist[y*width + x] = cdist[y*width + x] / normalizer;
		

		//delete[] chamfer;
		
	}
};

