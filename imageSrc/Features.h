#pragma once
#include "MyImage.h"

#define M_PI		3.14159265358979323846

void DrawCross(CByteImage& image, int x, int y, BYTE color[3]);
void DrawLine(CByteImage& canvas, int x1, int y1, int x2, int y2, BYTE val);
void DrawLine(CByteImage& canvas, int x1, int y1, int x2, int y2, BYTE R, BYTE G, BYTE B);
void SobelEdge(const CByteImage& imageIn, CByteImage& imageOut);
void CannyEdge(const CByteImage& imageIn, CByteImage& imageOut, int nThresholdHi, int nThresholdLo);
int  HoughLines(const CByteImage& imageIn, int nTNum, int nTVal, double resTheta, int numLine, double* pRho, double* pTheta);
int  HarrisCorner(const CByteImage& imageIn,  double dThreshold, double paramK, int nMaxCorner, double* posX, double* posY);


CDoubleImage _Gaussian5x5(const CIntImage& imageIn);

// jslee
void CCL_four(const CByteImage& imageIn, CByteImage& imageOut, int nWidthint, int nHeight, int color, int maskSize);
void CCL_eight(const CByteImage& imageIn, CByteImage& imageOut, int color);
void CCL_eight_long(const CByteImage& imageIn, CByteImage& imageOut, int color);
void ConnectedCoponentLabeling_small(const CByteImage& imageIn, CByteImage& imageOut, int nWidthint, int nHeight, int color, int numDel);

void EyelidSeg(const CByteImage& imageIn, CByteImage& imageOut, int nThreshold);

void BitReversal(const CByteImage& imageIn, CByteImage& imageOut, int inObj, int inBg, int outObj, int outBg);
void PupilSeg(const CByteImage& imageIn, CByteImage& imagePupil, CByteImage& imageOut);

void SobelEdge_Eyelid(const CByteImage& imageIn, CByteImage& imageOut, CByteImage& imageIris, Roi roi);
void CannyEdge_Eyelid(const CByteImage& imageIn, CByteImage& imageOut, CByteImage& imageIris, int nThresholdHi, int nThresholdLo, Roi roi);

void CannyEdge_Eyelid_s(const CByteImage& imageIn, CByteImage& imageOut, CByteImage& imageIris, int nThresholdHi, int nThresholdLo, Roi roi);
