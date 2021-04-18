#pragma once
#include "imageIO/MyImage.h"
#include <vector>
using namespace std;

#define M_PI		3.14159265358979323846

typedef struct point {
	union {
		struct {
			int x;
			int y;
			int seq;
		};
	};
} point;

class EHTelmt { //Ellipse(Cx,Cy,L,S)와 voting 값 담는 class
public:
	EHTelmt(int v, int x, int y, int a, int o) { // 포물선의 중심과 반지름에 따른 voting 값 넣는 공간
		vot = v;
		Cx = x;
		Cy = y;
		A = a;
		O = o;
	}
	bool operator>(const EHTelmt & cht)const { //sort 함수 사용 위해
		if (vot > cht.vot) return true;
		return false;
	}
	bool operator<(const EHTelmt & cht)const {
		if (vot < cht.vot) return true;
		return false;
	}
	int vot, Cx, Cy, A, O;
};


void AdaptiveBinarization(const CByteImage& imageIn, CByteImage& imageOut, CByteImage&imageIris);
void Otzu_Threshold(const CByteImage& imageIn, CByteImage& imageOut);


void Stepwise_Threshold(const CByteImage& imageIn, CByteImage& imageOut);

void ContourExtraction(const CByteImage& imageIn, CByteImage& imageOut);
void Contour_fitting(const CByteImage& imageIn, CByteImage& imageOut);
void NonMaxSuppression(const CByteImage& imageIn, CByteImage& imageOut);
void convex_hull(const CByteImage& imageIn, CByteImage& imageOut);
double distance(point tmp);
int cross_product(point m, point n);

Roi Detect_ROI(CByteImage& imageIris, CByteImage& imageOut, const CByteImage& imageIn, Roi roi);


void Crop_ROI(const CByteImage& imageIn, CByteImage& imageOut, Roi roi);
void ToPixel(const CByteImage& imageIn, CByteImage& imageOut);

void HoughParabolaDetction(CByteImage& imageObj, CByteImage& imageOut, parabolaComponent parabolaCompo);
void DrawParabola(CByteImage& imageOut, int x0, int y0, int a, int o);