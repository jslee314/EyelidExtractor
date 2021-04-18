#pragma once
#include "imageIO/MyImage.h"

void Graphcut(const CByteImage& imageIn, CByteImage& imageOut, CByteImage& GC_obj_seed, CByteImage& GC_bck_seed, Roi roi);



