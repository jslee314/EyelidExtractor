#pragma once
#include "MyImage.h"

void Erode(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut);
void Dilate(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut);

void Dilate_horiz(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut);


void ErodeG(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut);
void DilateG(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut);
