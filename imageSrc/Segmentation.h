#pragma once
#include "imageIO/MyImage.h"
#define MAX_CLUSTER 10

void Binarization(const CByteImage& imageIn, CByteImage& imageOut, int nThreshold, int objGL);
int BinarizationAuto(const CByteImage& imageIn, CByteImage& imageOut, int nThreshold);
void BinarizationBG(const CByteImage& imageIn, const CByteImage& imageBG, CByteImage& imageOut, int nThreshold);
void BinarizationBGCol(const CByteImage& imageIn, const CByteImage& imageBG, CByteImage& imageOut, int nThreshold);
void KMeansSegmentation(const CByteImage& imageIn, CByteImage& imageOut, int nCluster, BYTE meanR[], BYTE meanG[], BYTE meanB[]);
