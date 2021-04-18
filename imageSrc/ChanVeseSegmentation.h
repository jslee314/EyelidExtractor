// Robert Crandall
// Chan-Vese Segmentation

#ifndef CHAN_VESE_H
#define CHAN_VESE_H
#define IN_IMG(x, lo, hi) (x < lo)? lo : x > hi ? hi : x
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#define MAX(a,b)  ((a) < (b) ? (b) : (a))

#include "Image.h"
#include <cmath>

// Define structure containing parameters of
struct CVsetup
{
  double dt; // time step
  double h;  // pixel spacing
  double lambda1;
  double lambda2;
  double mu; // contour length weighting parameter
  double nu; // region area weighting parameter
  unsigned int p; // length weight exponent
};

// Compute gray level averages in foreground and background regions defined by level set function phi
void GetRegionAverages(Image<unsigned char>* img,
                       Image<double>* phi,
                       double epsilon,
                       double &c1,
                       double &c2);

// Compute coefficients needed in Chan-Vese segmentation algorithm given current level set function
void GetChanVeseCoefficients(Image<double>* phi,
                             struct CVsetup* pCVinputs,
                             unsigned int i,
                             unsigned int j,
                             double L,
                             double& F1,
                             double& F2,
                             double& F3,
                             double& F4,
                             double& F,
                             double& deltaPhi);


// Reinitialize a function to the signed distance function to its zero contour
void ReinitPhi(Image<double>* phiIn,
               Image<double>** psiOut,
               double dt,
               double h,
               unsigned int numIts);

// Main segmentation algorithm
void ChanVeseSegmentation(Image<unsigned char>* img,
                          Image<double>* phi0,
                          Image<double>** phi,
                          struct CVsetup* pCVinputs);

#endif