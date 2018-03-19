
#include "stdafx.h"
#include "Features.h"
#include "MyImageFunc.h"
#include <math.h>

#include "ImageFrameWndManager.h"


void DrawLine(CByteImage& canvas, int x1, int y1, int x2, int y2, BYTE val)
{
	ASSERT(canvas.GetChannel()==1);

	int xs, ys, xe, ye;
	if (x1 == x2) // 수직선
	{
		if (y1 < y2) {ys = y1; ye = y2;}
		else		 {ys = y2; ye = y1;}
		for (int r=ys ; r<=ye ; r++)
		{
			canvas.GetAt(x1, r) = val;
		}
		return;
	}

	double a = (double)(y2-y1)/(x2-x1); // 기울기
	int nHeight = canvas.GetHeight();

	if ((a>-1) && (a<1)) // 가로축에 가까움
	{
		if (x1 < x2) {xs = x1; xe = x2; ys = y1; ye = y2;}
		else		 {xs = x2; xe = x1; ys = y2; ye = y1;}
		for (int c=xs ; c<=xe ; c++)
		{
			int r = (int)(a*(c-xs) + ys + 0.5);
			if (r<0 || r>=nHeight)
				continue;
			canvas.GetAt(c, r) = val;
		}
	}
	else // 세로축에 가까움
	{
		double invA = 1.0 / a;
		if (y1 < y2) {ys = y1; ye = y2; xs = x1; xe = x2; }
		else		 {ys = y2; ye = y1; xs = x2; xe = x1; }
		for (int r=ys ; r<=ye ; r++)
		{
			int c = (int)(invA*(r-ys) + xs + 0.5);
			if (r<0 || r>=nHeight)
				continue;
			canvas.GetAt(c, r) = val;
		}
	}
}

void DrawLine(CByteImage& canvas, int x1, int y1, int x2, int y2, BYTE R, BYTE G, BYTE B)
{

	ASSERT(canvas.GetChannel()==3);

	int xs, ys, xe, ye;
	if (x1 == x2) // 수직선
	{
		if (y1 < y2) {ys = y1; ye = y2;}
		else		 {ys = y2; ye = y1;}
		for (int r=ys ; r<=ye ; r++)
		{
			canvas.GetAt(x1, r, 0) = B;
			canvas.GetAt(x1, r, 1) = G;
			canvas.GetAt(x1, r, 2) = R;
		}
		return;
	}

	double a = (double)(y2-y1)/(x2-x1); // 기울기
	int nHeight = canvas.GetHeight();

	if ((a>-1) && (a<1)) // 가로축에 가까움
	{
		if (x1 < x2) {xs = x1; xe = x2; ys = y1; ye = y2;}
		else		 {xs = x2; xe = x1; ys = y2; ye = y1;}
		for (int c=xs ; c<=xe ; c++)
		{
			int r = (int)(a*(c-xs) + ys + 0.5);
			if (r<0 || r>=nHeight)
				continue;
			canvas.GetAt(c, r, 0) = B;
			canvas.GetAt(c, r, 1) = G;
			canvas.GetAt(c, r, 2) = R;
		}
	}
	else // 세로축에 가까움
	{
		double invA = 1.0 / a;
		if (y1 < y2) {ys = y1; ye = y2; xs = x1; xe = x2; }
		else		 {ys = y2; ye = y1; xs = x2; xe = x1; }
		for (int r=ys ; r<=ye ; r++)
		{
			int c = (int)(invA*(r-ys) + xs + 0.5);
			if (r<0 || r>=nHeight)
				continue;
			canvas.GetAt(c, r, 0) = B;
			canvas.GetAt(c, r, 1) = G;
			canvas.GetAt(c, r, 2) = R;
		}
	}
}



int HoughLines(const CByteImage& imageIn, int nTNum, int nTVal, double resTheta, int numLine,
	double* pRho, double* pTheta)
{
	int nWidth	= imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nWStep	= imageIn.GetWStep();

	int numRhoH = (int)(sqrt((double)(nWidth*nWidth + nHeight*nHeight))); // 영상 대각선의 길이
	int numRho = numRhoH*2; // rho의 음수 영역을 위해 2를 곱함
	
	
	int numThe = 180 / resTheta;
	int numTrans = numRho*numThe; // rho와 theta 조합의 출현 횟수를 저장하는 공간

	double* sinLUT = new double[numThe]; // sin 함수 룩업 테이블
	double* cosLUT = new double[numThe]; // cos 함수 룩업 테이블
	
	double toRad = M_PI/numThe;
	for (int theta=0 ; theta<numThe ; theta++)
	{
		sinLUT[theta] = (double)sin(theta*toRad);
		cosLUT[theta] = (double)cos(theta*toRad);
	}

	int* pCntTrans = new int[numTrans];
	memset(pCntTrans, 0, numTrans*sizeof(int));

	for (int r=0 ; r<nHeight ; r++)
	{
		BYTE* pIn = imageIn.GetPtr(r);
		for (int c=0 ; c<nWidth ; c++)
		{
			if (pIn[c] > nTVal) // 경계선 픽셀
			{


				for (int theta=0 ; theta<numThe ; theta++) 
				{
					int rho = (int)(c*sinLUT[theta] + r*cosLUT[theta] + numRhoH + 0.5);
					pCntTrans[rho*numThe+theta]++;
				}



			}
		}
	}

	// nThreshold을넘는 결과 저장
	int nLine = 0;
	for (int i=0 ; i<numTrans && nLine<numLine ; i++)
	{
		if (pCntTrans[i] > nTNum)
		{
			pRho[nLine]	  = (int)(i/numThe); // rho의 인덱스
			pTheta[nLine] = (i - pRho[nLine]*numThe)*resTheta; //theta 의 인덱스
			pRho[nLine]	  -= numRhoH; // 음수 값이 차지하는 위치만큼 뺄셈
			nLine++;
		}
	}

	delete [] pCntTrans;
	delete [] sinLUT;
	delete [] cosLUT;

	return nLine;
}



void SobelEdge(const CByteImage& imageIn, CByteImage& imageOut)
{
	int nWidth	= imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	imageOut = CByteImage(nWidth, nHeight);
	imageOut.SetConstValue(0);

	int nWStep = imageIn.GetWStep();

	// Sobel 마스크
	int Gx[9], Gy[9];
	Gx[0] = -1; Gx[1] = 0; Gx[2] = 1;
	Gx[3] = -2; Gx[4] = 0; Gx[5] = 2;
	Gx[6] = -1; Gx[7] = 0; Gx[8] = 1;

	Gy[0] =  1; Gy[1] =  2; Gy[2] =  1;
	Gy[3] =  0; Gy[4] =  0; Gy[5] =  0;
	Gy[6] = -1; Gy[7] = -2; Gy[8] = -1;

	BYTE* pIn  = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();

	for (int r=1 ; r<nHeight-1 ; r++) // 영상 경계는 제외
	{
	for (int c=1 ; c<nWidth-1 ; c++) // 영상 경계는 제외
	{
		int sumX = 0;
		int sumY = 0;
		for (int y=0 ; y<3 ; y++)
		{
		for (int x=0 ; x<3 ; x++)
		{
			int py = r-1+y;
			int px = c-1+x;
			if (px>=0 && px<nWidth && py>=0 && py<nHeight)
			{
				sumX += Gx[y*3+x]*pIn[py*nWStep+px];
				sumY += Gy[y*3+x]*pIn[py*nWStep+px];
			}
		}
		}
//		pOut[c] = sqrt((double)(sumX*sumX + sumY*sumY)/32); // 경계선의 세기
		pOut[c] = MIN(sqrt((double)(sumX*sumX + sumY*sumY)/4), 255);
	}
	pOut += nWStep;
	}
}



CDoubleImage _Gaussian5x5(const CIntImage& imageIn)
{
	int nWidth	= imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nWStep = imageIn.GetWStep();

	CDoubleImage imageGss(nWidth, nHeight);
	imageGss.SetConstValue(0);
	int nWStepG = imageGss.GetWStep();

	int nWStep2 = 2*nWStep;

	for (int r=2 ; r<nHeight-2 ; r++)
	{
		double* pGss = imageGss.GetPtr(r);
		int*    pIn  = imageIn.GetPtr(r);
		for (int c=2 ; c<nWidth-2 ; c++)
		{
			pGss[c] = (2*(pIn[c-nWStep2-2] + pIn[c-nWStep2+2] + pIn[c+nWStep2-2] + pIn[c+nWStep2+2]) + 
					   4*(pIn[c-nWStep2-1] + pIn[c-nWStep2+1] + pIn[c-nWStep-2] + pIn[c-nWStep+2] +
						 pIn[c+nWStep-2] + pIn[c+nWStep+2] + pIn[c+nWStep2-1] + pIn[c+nWStep2+1]) +
					   5*(pIn[c-nWStep2] + pIn[c-2] + pIn[c+2] + pIn[c+nWStep2]) + 
					   9*(pIn[c-nWStep-1] + pIn[c-nWStep+1] + pIn[c+nWStep-1] + pIn[c+nWStep+1]) + 
					   12*(pIn[c-nWStep] + pIn[c+nWStep] + pIn[c-1] + pIn[c+1]) + 
					   15*(pIn[c]))/159.0;
		}
	}
	
	return imageGss;
}

// anisotropic diffusion filter(비등방성 확산 필터)
void AnisotropicDiffusion(const CByteImage& imageIn, CByteImage& imageOut, float lambda, float k, int iter){
	//  IppByteImage& imgSrc, IppFloatImage& imgDst

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	CByteImage imageTmp = CByteImage(nWidth, nHeight);

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();
	

	//-------------------------------------------------------------------------
	// iter 횟수만큼 비등방성 확산 알고리즘 수행
	//-------------------------------------------------------------------------

	register int i, x, y;
	float gradn, grads, grade, gradw;
	float gcn, gcs, gce, gcw;
	float k2 = k * k;

	for (i = 0; i < iter; i++)
	{
		for (y = 1; y < nHeight - 1; y++) {
			for (x = 1; x < nWidth - 1; x++)
			{
				// 동서남북 방향의 Edge Gradient 구하기
				gradn = pIn[(y - 1)*nWidth + x] - pIn[y*nWidth + x];
				grads = pIn[(y + 1)*nWidth + x] - pIn[y*nWidth + x];
				grade = pIn[y*nWidth + (x - 1)] - pIn[y*nWidth + x];
				gradw = pIn[y*nWidth + (x + 1)] - pIn[y*nWidth + x];

				// 각 방향에서 사용자가 입력한 정규화상수(k2)를 이용해 정규화 
				gcn = gradn / (1.0f + gradn * gradn / k2);
				gcs = grads / (1.0f + grads * grads / k2);
				gce = grade / (1.0f + grade * grade / k2);
				gcw = gradw / (1.0f + gradw * gradw / k2);

				// 그 합과 사용자가 입력한 가중치(Lamda)를 더해 출력
				pOut[y*nWidth + x] = pIn[y*nWidth + x] + lambda*(gcn + gcs + gce + gcw);
			}
		}

		// 버퍼 복사
		if (i < iter - 1) {
			//memcpy(imageIn.GetPixels(), imageOut.GetPixels(), sizeof(float) * nWidth * nHeight);


		}
			
	}
}

void CannyEdge(const CByteImage& imageIn, CByteImage& imageOut, int nThresholdHi, int nThresholdLo)
{
	int nWidth	= imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nWStep = imageIn.GetWStep();

	// 가우시안 마스크
	CDoubleImage  imageGss(nWidth, nHeight);
	imageGss = _Gaussian5x5(imageIn);

	// 소벨 마스크
	int Gx[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
	int Gy[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};

	CDoubleImage  imageMag(nWidth, nHeight);
	CByteImage imageAng(nWidth, nHeight);
	int nWStepG = imageGss.GetWStep();
	int nWStepA = imageAng.GetWStep();

	double* pGss = imageGss.GetPtr();

	// 미분 구하기
	for (int r=0 ; r<nHeight ; r++)
	{
	double* pMag = imageMag.GetPtr(r);
	BYTE*   pAng = imageAng.GetPtr(r);
	for (int c=0 ; c<nWidth ; c++)
	{
		double sumX = 0.0;
		double sumY = 0.0;
		for (int y=0 ; y<3 ; y++)
		{
			for (int x=0 ; x<3 ; x++)
			{
				int py = r-1+y;
				int px = c-1+x;
				if (px>=0 && px<nWidth && py>=0 && py<nHeight)
				{
					sumX += Gx[y*3+x]*pGss[py*nWStepG+px];
					sumY += Gy[y*3+x]*pGss[py*nWStepG+px];
				}
			}
		}

		pMag[c] = sqrt(sumX*sumX + sumY*sumY); // 경계선의 세기
		double theta;					 // 경계선의 수직 방향
		if (pMag[c] == 0)
		{
			if(sumY == 0)
			{
				theta = 0;
			}
			else
			{
				theta = 90;
			}
		}
		else
		{
			theta = atan2((float)sumY, (float)sumX)*180.0/M_PI;
		}

		if ((theta > -22.5 && theta < 22.5) || theta > 157.5 || theta < -157.5)
		{
			pAng[c] = 0;
		}
		else if ((theta >= 22.5 && theta < 67.5) || (theta >= -157.5 && theta < -112.5))
		{
			pAng[c] = 45;
		}
		else if ((theta >= 67.5 && theta <= 112.5) || (theta >= -112.5 && theta <= -67.5))
		{
			pAng[c] = 90;
		}
		else
		{
			pAng[c] = 135;
		}
	} // 열 이동 끝
	} // 행 이동 끝

	// 비 최대값 억제
	CByteImage imageCand(nWidth, nHeight);
	imageCand.SetConstValue(0);

	for (int r = 1; r<nHeight - 1; r++)	{
		BYTE*	pCand = imageCand.GetPtr(r);
		double* pMag = imageMag.GetPtr(r);
		BYTE*   pAng = imageAng.GetPtr(r);
		for (int c = 1; c<nWidth - 1; c++)
		{
			switch (pAng[c])
			{
			case 0:		// 0도 방향 비교
				if (pMag[c] > pMag[c - 1] && pMag[c] > pMag[c + 1])
				{
					pCand[c] = 255;
				}
				break;
			case 45:	// 45도 방향 비교
				if (pMag[c] > pMag[c - nWStepG + 1] && pMag[c] > pMag[c + nWStepG - 1])
				{
					pCand[c] = 255;
				}
				break;
			case 90:		// 90도 방향 비교
				if (pMag[c] > pMag[c - nWStepG] && pMag[c] > pMag[c + nWStepG])
				{
					pCand[c] = 255;
				}
				break;
			case 135:	// 135도 방향 비교
				if (pMag[c] > pMag[c - nWStepG - 1] && pMag[c] > pMag[c + nWStepG + 1])
				{
					pCand[c] = 255;
				}
				break;
			}
		} // 열 이동 끝
	} // 행 이동 끝

	imageCand.SaveImage("Cand.bmp");
	//imageOut = imageCand;
	
	
	// 문턱값 검사
	imageOut = CByteImage(nWidth, nHeight);
	imageOut.SetConstValue(0);
	for (int r = 1; r<nHeight - 1; r++)
	{
		BYTE*	pOut = imageOut.GetPtr(r);
		BYTE*	pCand = imageCand.GetPtr(r);
		double* pMag = imageMag.GetPtr(r);
		BYTE*   pAng = imageAng.GetPtr(r);

		for (int c = 1; c<nWidth - 1; c++){
			if (pCand[c])
			{
				if (pMag[c] > nThresholdHi)
				{
					pOut[c] = 255;
				}
				else if (pMag[c] > nThresholdLo) // 연결된 픽셀 검사
				{
					bool bIsEdge = true;
					switch (pAng[c])
					{
					case 0:		// 90도 방향 검사
						if ((pMag[c - nWStepG] > nThresholdHi) ||
							(pMag[c + nWStepG] > nThresholdHi))
						{
							pOut[c] = 255;
						}
						break;
					case 45:	// 135도 방향 검사
						if ((pMag[c - nWStepG - 1] > nThresholdHi) ||
							(pMag[c + nWStepG + 1] > nThresholdHi))
						{
							pOut[c] = 255;
						}
						break;
					case 90:		// 0도 방향 검사
						if ((pMag[c - 1] > nThresholdHi) ||
							(pMag[c + 1] > nThresholdHi))
						{
							pOut[c] = 255;
						}
						break;
					case 135:	// 45도 방향 검사
						if ((pMag[c - nWStepG + 1] > nThresholdHi) ||
							(pMag[c + nWStepG - 1] > nThresholdHi))
						{
							pOut[c] = 255;
						}
						break;
					}
				}
			}
		} // 열 이동 끝
	} // 행 이동 끝
	

}

int HarrisCorner(const CByteImage& imageIn,  double dThreshold, double paramK, int nMaxCorner, double* posX, double* posY)
{
	int nWidth	= imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nWStep = imageIn.GetWStep();

	CIntImage imageDxx(nWidth, nHeight); imageDxx.SetConstValue(0);
	CIntImage imageDxy(nWidth, nHeight); imageDxy.SetConstValue(0);
	CIntImage imageDyy(nWidth, nHeight); imageDyy.SetConstValue(0);
	int nWStepD = imageDxx.GetWStep();

	// 미분 계산
	int dx, dy;

	for (int r=1 ; r<nHeight-1 ; r++)
	{
		BYTE* pIn = imageIn.GetPtr(r);
		int* pDxx = imageDxx.GetPtr(r);
		int* pDxy = imageDxy.GetPtr(r);
		int* pDyy = imageDyy.GetPtr(r);
		for (int c=1 ; c<nWidth-1 ; c++)
		{
			dx = pIn[c-nWStep-1] + 2*pIn[c-1] + pIn[c+nWStep-1] 
			   - pIn[c-nWStep+1] - 2*pIn[c+1] - pIn[c+nWStep+1];
			dy = pIn[c-nWStep-1] + 2*pIn[c-nWStep] + pIn[c-nWStep+1] 
			   - pIn[c+nWStep-1] - 2*pIn[c+nWStep] - pIn[c+nWStep+1];
			pDxx[c] = dx*dx;
			pDxy[c] = dx*dy;
			pDyy[c] = dy*dy;
		}
	}

	// 가우시안 필터링
	CDoubleImage imageGDxx = _Gaussian5x5(imageDxx);
	CDoubleImage imageGDxy = _Gaussian5x5(imageDxy);
	CDoubleImage imageGDyy = _Gaussian5x5(imageDyy);

	// 코너 응답 함수
	CDoubleImage imageCornerScore(nWidth, nHeight);

	for (int r=2 ; r<nHeight-2 ; r++)
	{
		double *pScore = imageCornerScore.GetPtr(r);
		double* pGDxx = imageGDxx.GetPtr(r);
		double* pGDxy = imageGDxy.GetPtr(r);
		double* pGDyy = imageGDyy.GetPtr(r);
		for (int c=2 ; c<nWidth-2 ; c++)
		{
			pScore[c] = (pGDxx[c]*pGDyy[c] - pGDxy[c]*pGDxy[c]) 
				- paramK*(pGDxx[c]+pGDyy[c])*(pGDxx[c]+pGDyy[c]);
		}
	}

	// 지역 최대값 추출
	int numCorner = 0;
	for (int r=2 ; r<nHeight-2 ; r++)
	{
		double *pScore = imageCornerScore.GetPtr(r);
		for (int c=2 ; c<nWidth-2 ; c++)
		{
			if (pScore[c] > dThreshold)
			{
				if (pScore[c] > pScore[c-nWStepD] &&
					pScore[c] > pScore[c+nWStepD] &&
					pScore[c] > pScore[c-1] &&
					pScore[c] > pScore[c+1])
				{
					posX[numCorner] = c;
					posY[numCorner] = r;
					numCorner++;

					if (numCorner >= nMaxCorner)
						return nMaxCorner;
				}
			}
		}
	}

	return numCorner;
}

void DrawCross(CByteImage& image, int x, int y, BYTE color[3])
{
	image.GetAt(x, y, 0) = color[0];
	image.GetAt(x, y, 1) = color[1];
	image.GetAt(x, y, 2) = color[2];

	image.GetAt(x, y-1, 0) = color[0];
	image.GetAt(x, y-1, 1) = color[1];
	image.GetAt(x, y-1, 2) = color[2];

	image.GetAt(x, y+1, 0) = color[0];
	image.GetAt(x, y+1, 1) = color[1];
	image.GetAt(x, y+1, 2) = color[2];

	image.GetAt(x-1, y, 0) = color[0];
	image.GetAt(x-1, y, 1) = color[1];
	image.GetAt(x-1, y, 2) = color[2];

	image.GetAt(x, y+1, 0) = color[0];
	image.GetAt(x, y+1, 1) = color[1];
	image.GetAt(x, y+1, 2) = color[2];
}

void CCL_four(const CByteImage& imageIn, CByteImage& imageOut, int nWidthint, int nHeight, int color, int maskSize) {

	int w = nWidthint;
	int h = nHeight;

	int *hist = new int[9000];
	int* map = new int[w * h];
	int eq_tbl[90000 * 2];

	memset(hist, 0, sizeof(int) * 9000);
	memset(map, 0, sizeof(int) * w * h);
	memset(eq_tbl, 0, sizeof(int) * 90000 * 2);

	int label = 0, maxl, minl, min_eq;
	int labelingVal = 0;


	for (int y = maskSize-1; y<h- maskSize-1; y++) {
		BYTE* pIn = imageIn.GetPtr(y);
		for (int x = maskSize-1; x<w- maskSize-1; x++) {
			if (pIn[x] == color) {

				// (1) 위와 왼쪽 픽셀 레이블이 존재할 경우
				if ((map[(y - 1) * w + x] != 0) && (map[y * w + (x - 1)] != 0)) {

					//	(1-1) 두 레이블이 같은 경우
					if (map[(y - 1) * w + x] == map[y * w + (x - 1)]) {
						map[y * w + x] = map[(y - 1) * w + x];
					}
					// (1-2) 두 레이블이 서로 다른 경우, 작은 레이블을 부여
					else {
						maxl = max(map[(y - 1) * w + x], map[y * w + (x - 1)]);
						minl = min(map[(y - 1) * w + x], map[y * w + (x - 1)]);
						map[y * w + x] = minl;
						// 등가 테이블 조정
						min_eq = min(eq_tbl[maxl * 2 + 1], eq_tbl[minl * 2 + 1]);
						eq_tbl[maxl * 2 + 1] = min_eq;
						eq_tbl[minl * 2 + 1] = min_eq;
					}
				}
				// (2) 바로 위 픽셀에만 레이블이 존재할 경우
				else if (map[(y - 1) * w + x] != 0) {
					map[y * w + x] = map[(y - 1) * w + x];
				}
				// (3) 바로 왼쪽 픽셀이만 레이블이 존재할 경우
				else if (map[y * w + (x - 1)] != 0) {
					map[y * w + x] = map[y * w + (x - 1)];
				}
				// (4) 이웃에 레이블이 존재하지 않으면 새로운 레이블을 부여
				else {
					label++;
					map[y * w + x] = label;
					eq_tbl[label * 2 + 0] = label;
					eq_tbl[label * 2 + 1] = label;
				}
			}
		}//w
	}//h


	 // 등가 테이블 정리----------------------
	int temp;
	for (int m = 1; m <= label; m++) {
		temp = eq_tbl[m * 2 + 1];

		if (temp != eq_tbl[m * 2 + 0])
			eq_tbl[m * 2 + 1] = eq_tbl[temp * 2 + 1];
	}

	// 등가 테이블의 레이블을 1부터 차례대로 증가시키기
	int* hash = new int[label + 1];
	memset(hash, 0, sizeof(int) * (label + 1));

	for (int n = 1; n <= label; n++)
		hash[eq_tbl[n * 2 + 1]] = eq_tbl[n * 2 + 1];

	int cnt = 1;
	for (int l = 1; l <= label; l++)
		if (hash[l] != 0)
			hash[l] = cnt++;

	for (int f = 1; f <= label; f++)
		eq_tbl[f * 2 + 1] = hash[eq_tbl[f * 2 + 1]];

	delete[] hash;

	// 두번째 스캔 시작.
	for (int p = maskSize-1; p < h- maskSize-1; p++) {
		BYTE* pIn = imageIn.GetPtr(p);
		for (int q = maskSize-1; q < w- maskSize-1; q++) {
			if (map[p * w + q] != 0) {

				temp = map[p * w + q];
				labelingVal = eq_tbl[temp * 2 + 1];
				pIn[q] = labelingVal;
				hist[labelingVal]++;

			}
		} // w
	}// h

	 // 제일 많은 컴포넌트를 선택한다.--------------------
	int max = 0;
	int val;
	for (int p = 0; p < 9000; p++) {
		if (hist[p] != 0 && max < hist[p]) {
			max = hist[p];
			val = p;
		}
	}

	for (int y = maskSize-1; y<h- maskSize-1; y++) {
		BYTE* pIn = imageIn.GetPtr(y);
		BYTE* pOut = imageOut.GetPtr(y);
		for (int x = maskSize-1; x<w- maskSize-1; x++) {
			if (pIn[x] == val) {
				pOut[x] = color;
			}
			else {
				pOut[x] = fabs(255-color);
			}
		}
	}

	delete[] hist;
	delete[] map;

}

void CCL_eight(const CByteImage& imageIn, CByteImage& imageOut, int color) {

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();

	int* map = new int[nWidth * nHeight];	// 연결된 성분끼리 라벨링된 이미지
	int *hist = new int[9000];				// 각 레이블 당 픽셀의 갯수
	int eq_tbl[90000 * 2];

	memset(hist, 0, sizeof(int) * 9000);
	memset(map, 0, sizeof(int) * nWidth * nHeight);
	memset(eq_tbl, 0, sizeof(int) * 90000 * 2);

	int label = 0, maxl, minl, min_eq;
	int labelingVal = 0;

	

	for (int y = 1; y<nHeight-1; y++) {
		for (int x = 1; x<nWidth-1; x++) {
			int index = y*nWidth + x;

			if (pIn[index] == color) {
				
				// (1) 위와 왼쪽 픽셀 레이블이 존재할 경우
				if ((map[(y - 1) *nWidth + x] != 0) && (map[y *nWidth + (x - 1)] != 0)) {

					//	(1-1) 두 레이블이 같은 경우
					if (map[(y - 1) *nWidth + x] == map[y *nWidth + (x - 1)]) {
						map[index] = map[(y - 1) *nWidth + x];
					}

					// (1-2) 두 레이블이 서로 다른 경우, 작은 레이블을 부여
					else {
						maxl = max(map[(y - 1) *nWidth + x], map[y *nWidth + (x - 1)]);
						minl = min(map[(y - 1) *nWidth + x], map[y *nWidth + (x - 1)]);
						map[index] = minl;
						// 등가 테이블 조정
						min_eq = min(eq_tbl[maxl * 2 + 1], eq_tbl[minl * 2 + 1]);
						eq_tbl[maxl * 2 + 1] = min_eq;
						eq_tbl[minl * 2 + 1] = min_eq;
					}
				}
				// (2) 위 픽셀만 레이블이 존재할 경우, 위 픽셀로 레이블 부여
				else if (map[(y - 1)*nWidth + x] != 0) {
					map[index] = map[(y - 1)*nWidth + x];
				}
				// (3) 왼쪽 픽셀만 레이블이 존재할 경우, 왼쪽 픽셀로 레이블 부여
				else if (map[y *nWidth + (x - 1)] != 0) {
					map[index] = map[y *nWidth + (x - 1)];
				}
				// (4) 위왼대각선 픽셀만 레이블이 존재할 경우, 위왼대각선 픽셀로 레이블 부여
				else if (map[(y - 1)*nWidth + (x - 1)] != 0) {
					map[index] = map[(y - 1) *nWidth + (x - 1)];
				}
				// (5) 위왼대각선 픽셀만 레이블이 존재할 경우, 위왼대각선 픽셀로 레이블 부여
				else if (map[(y - 1) * nWidth + (x + 1)] != 0) {
					map[index] = map[(y - 1) * nWidth + (x + 1)];
				}


				// (6) 이웃에 레이블이 존재하지 않으면, 새로운 레이블을 부여
				//else if (map[(y - 1)*nWidth + (x - 1)] != 0 && map[y*nWidth + (x - 1)] != 0 && map[(y - 1) *nWidth + x] != 0) {
				else{	
					label++;
					map[index] = label;
					eq_tbl[label * 2 + 0] = label;
					eq_tbl[label * 2 + 1] = label;
				}

			}
		}//w
	}//h


	 // 등가 테이블 정리----------------------
	int temp;
	for (int m = 1; m <= label; m++) {
		temp = eq_tbl[m * 2 + 1];

		if (temp != eq_tbl[m * 2 + 0])
			eq_tbl[m * 2 + 1] = eq_tbl[temp * 2 + 1];
	}

	// 등가 테이블의 레이블을 1부터 차례대로 증가시키기
	int* hash = new int[label + 1];
	memset(hash, 0, sizeof(int) * (label + 1));

	for (int n = 1; n <= label; n++)
		hash[eq_tbl[n * 2 + 1]] = eq_tbl[n * 2 + 1];

	int cnt = 1;
	for (int l = 1; l <= label; l++)
		if (hash[l] != 0)
			hash[l] = cnt++;

	for (int f = 1; f <= label; f++)
		eq_tbl[f * 2 + 1] = hash[eq_tbl[f * 2 + 1]];

	delete[] hash;

	// 두번째 스캔 시작.
	for (int p = 0; p < nHeight; p++) {
		BYTE* pIn = imageIn.GetPtr(p);
		for (int q = 0; q < nWidth; q++) {
			if (map[p * nWidth + q] != 0) {

				temp = map[p * nWidth + q];
				labelingVal = eq_tbl[temp * 2 + 1];
				pIn[q] = labelingVal;
				hist[labelingVal]++;

			}
		} // w
	}// h

	 // 제일 많은 컴포넌트를 선택한다.--------------------
	int max = 0;
	int val;
	for (int p = 0; p < 9000; p++) {
		if (hist[p] != 0 && max < hist[p]) {
			max = hist[p];
			val = p;
		}
	}

	 	//for (int y=0; y<nHeight; y++) {
	 	//	for (int x=0; x<nWidth; x++) {
	 
	 	//		// 픽셀수가 50이하면 지워버리자
	 	//		if(hist[map[y*nWidth +x]] < 5 ){
	 	//			pOut[y*nWidth +x] = BACKGROUND;
	 	//		}
	 	//	}
	 	//}
	for (int y = 0; y<nHeight; y++) {
		BYTE* pIn = imageIn.GetPtr(y);
		BYTE* pOut = imageOut.GetPtr(y);
		for (int x = 0; x<nWidth; x++) {
			if (pIn[x] == val) {
				pOut[x] = color;
			}
			else {
				pOut[x] = fabs(255 - color);
			}
		}
	}
	


	//ShowImage(imageOut, "22");


	delete[] hist;
	delete[] map;




}

void CCL_eight_long(const CByteImage& imageIn, CByteImage& imageOut, int color) {

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();

	int* map = new int[nWidth * nHeight];	// 연결된 성분끼리 라벨링된 이미지
	int *hist = new int[9000];				// 각 레이블 당 픽셀의 갯수
	Roi *Wlabel = new Roi[9000];			// 각 레이블 당 roi
	int *Whist = new int[9000];				// 각 레이블 당 최대 너비
	int eq_tbl[90000 * 2];

	memset(hist, 0, sizeof(int) * 9000);
	memset(Wlabel, 0, sizeof(Roi) * 9000);
	memset(Whist, 0, sizeof(int) * 9000);
	memset(map, 0, sizeof(int) * nWidth * nHeight);
	memset(eq_tbl, 0, sizeof(int) * 90000 * 2);

	int label = 0, maxl, minl, min_eq;
	int labelingVal = 0;

	for (int y = 0; y<nHeight; y++) {
		for (int x = 0; x<nWidth; x++) {
			int index = y*nWidth + x;

			if (pIn[index] == color) {

				// (1) 위와 왼쪽 픽셀 레이블이 존재할 경우
				if ((map[(y - 1) *nWidth + x] != 0) && (map[y *nWidth + (x - 1)] != 0)) {

					//	(1-1) 두 레이블이 같은 경우
					if (map[(y - 1) *nWidth + x] == map[y *nWidth + (x - 1)]) {
						map[index] = map[(y - 1) *nWidth + x];
					}

					// (1-2) 두 레이블이 서로 다른 경우, 작은 레이블을 부여
					else {
						maxl = max(map[(y - 1) *nWidth + x], map[y *nWidth + (x - 1)]);
						minl = min(map[(y - 1) *nWidth + x], map[y *nWidth + (x - 1)]);
						map[index] = minl;
						// 등가 테이블 조정
						min_eq = min(eq_tbl[maxl * 2 + 1], eq_tbl[minl * 2 + 1]);
						eq_tbl[maxl * 2 + 1] = min_eq;
						eq_tbl[minl * 2 + 1] = min_eq;
					}
				}
				// (2) 위 픽셀만 레이블이 존재할 경우, 위 픽셀로 레이블 부여
				else if (map[(y - 1)*nWidth + x] != 0) {
					map[index] = map[(y - 1)*nWidth + x];
				}
				// (3) 왼쪽 픽셀만 레이블이 존재할 경우, 왼쪽 픽셀로 레이블 부여
				else if (map[y *nWidth + (x - 1)] != 0) {
					map[index] = map[y *nWidth + (x - 1)];
				}
				// (4) 위왼대각선 픽셀만 레이블이 존재할 경우, 위왼대각선 픽셀로 레이블 부여
				else if (map[(y - 1)*nWidth + (x - 1)] != 0) {
					map[index] = map[(y - 1) *nWidth + (x - 1)];
				}
				// (5) 위왼대각선 픽셀만 레이블이 존재할 경우, 위왼대각선 픽셀로 레이블 부여
				else if (map[(y - 1) * nWidth + (x + 1)] != 0) {
					map[index] = map[(y - 1) * nWidth + (x + 1)];
				}
				// (6) 이웃에 레이블이 존재하지 않으면, 새로운 레이블을 부여
				//else if (map[(y - 1)*nWidth + (x - 1)] != 0 && map[y*nWidth + (x - 1)] != 0 && map[(y - 1) *nWidth + x] != 0) {
				else {
					label++;
					map[index] = label;
					eq_tbl[label * 2 + 0] = label;
					eq_tbl[label * 2 + 1] = label;
				}

			}
		}//w
	}//h


	 // 등가 테이블 정리----------------------
	int temp;
	for (int m = 1; m <= label; m++) {
		temp = eq_tbl[m * 2 + 1];

		if (temp != eq_tbl[m * 2 + 0])
			eq_tbl[m * 2 + 1] = eq_tbl[temp * 2 + 1];
	}

	// 등가 테이블의 레이블을 1부터 차례대로 증가시키기
	int* hash = new int[label + 1];
	memset(hash, 0, sizeof(int) * (label + 1));

	for (int n = 1; n <= label; n++)
		hash[eq_tbl[n * 2 + 1]] = eq_tbl[n * 2 + 1];

	int cnt = 1;
	for (int l = 1; l <= label; l++)
		if (hash[l] != 0)
			hash[l] = cnt++;

	for (int f = 1; f <= label; f++)
		eq_tbl[f * 2 + 1] = hash[eq_tbl[f * 2 + 1]];

	delete[] hash;


	// 두번째 스캔 시작.
	for (int p = 0; p < nHeight; p++){
		for (int q = 0; q < nWidth; q++){
			int index = p * nWidth + q;

			if (map[index] != 0) {

				temp = map[index];
				labelingVal = eq_tbl[temp * 2 + 1];
				pIn[index] = labelingVal;
				hist[labelingVal]++;

			}
		} // w
	}// h

	 // 제일 많은 컴포넌트를 선택한다.
	int max = 0;
	int val;
	for (int p = 0; p < 9000; p++) {
		if (hist[p] != 0 && max < hist[p]){
			max = hist[p];
			val = p;
		}
	}

	for (int p = 0; p < 9000; p++) {
		Wlabel[p].stX = nWidth;	
		Wlabel[p].stY = nHeight;
		Wlabel[p].endX = 0;	
		Wlabel[p].endY = 0;
	}
	
	int nLabel=0;
	for (int y = 0; y < nHeight; y++){
		for (int x = 0; x < nWidth; x++){

			int index = y*nWidth + x;
			if (pIn[index] != 0){

				nLabel = pIn[index];			// 특정 레이블 값을 temp에 저장

				if (Wlabel[nLabel].stY > y) {
					Wlabel[nLabel].stY = y;
				}
				if (Wlabel[nLabel].stX > x) {
					Wlabel[nLabel].stX = x;
				}

				if (Wlabel[nLabel].endY < y) {
					Wlabel[nLabel].endY = y;
				}
				if (Wlabel[nLabel].endX < x) {
					Wlabel[nLabel].endX = x;
				}
			}
		}
	}


	for (int p = 0; p < 9000; p++) {
		Whist[p] = Wlabel[p].endX - Wlabel[p].stX;
	}

	int Wval = 0;;
	max = 0;
	for (int p = 0; p < 9000; p++) {
		if (Whist[p] != 0 && max < Whist[p]) {
			max = Whist[p];
			Wval = p;
		}
	}


	//for (int y=0; y<nHeight; y++) {
	//	for (int x=0; x<nWidth; x++) {
	//		// 픽셀수가 50이하면 지워버리자
	//		if(hist[map[y*nWidth +x]] < 5 ){
	//			pOut[y*nWidth +x] = BACKGROUND;
	//		}
	//	}
	//}



	for (int y = 0; y<nHeight; y++){
		for (int x = 0; x<nWidth; x++){
			int index = y*nWidth + x;

			if(pIn[index] == Wval){
				pOut[index] = color;
			}
			else{
				pOut[index] = fabs(255 - color);
			}
		}
	}


	delete[] hist;
	delete[] map;


}

void ConnectedCoponentLabeling_small(const CByteImage& imageIn, CByteImage& imageOut, int nWidthint, int nHeight, int color, int numDel) {

	int w = nWidthint;
	int h = nHeight;

	int *hist = new int[9000];
	int* map = new int[w * h];
	int eq_tbl[90000 * 2];

	memset(hist, 0, sizeof(int) * 9000);
	memset(map, 0, sizeof(int) * w * h);
	memset(eq_tbl, 0, sizeof(int) * 90000 * 2);

	int label = 0, maxl, minl, min_eq;
	int labelingVal = 0;

	for (int y = 1; y<h; y++) {
		BYTE* pIn = imageIn.GetPtr(y);

		for (int x = 1; x<w; x++) {
			if (pIn[x] == color) {

				// (1) 위와 왼쪽 픽셀 레이블이 존재할 경우
				if ((map[(y - 1) * w + x] != 0) && (map[y * w + (x - 1)] != 0)) {

					//	(1-1) 두 레이블이 같은 경우
					if (map[(y - 1) * w + x] == map[y * w + (x - 1)]) {
						map[y * w + x] = map[(y - 1) * w + x];
					}
					// (1-2) 두 레이블이 서로 다른 경우, 작은 레이블을 부여
					else {
						maxl = max(map[(y - 1) * w + x], map[y * w + (x - 1)]);
						minl = min(map[(y - 1) * w + x], map[y * w + (x - 1)]);
						map[y * w + x] = minl;
						// 등가 테이블 조정
						min_eq = min(eq_tbl[maxl * 2 + 1], eq_tbl[minl * 2 + 1]);
						eq_tbl[maxl * 2 + 1] = min_eq;
						eq_tbl[minl * 2 + 1] = min_eq;
					}
				}
				// (2) 바로 위 픽셀에만 레이블이 존재할 경우
				else if (map[(y - 1) * w + x] != 0) {
					map[y * w + x] = map[(y - 1) * w + x];
				}
				// (3) 바로 왼쪽 픽셀이만 레이블이 존재할 경우
				else if (map[y * w + (x - 1)] != 0) {
					map[y * w + x] = map[y * w + (x - 1)];
				}
				// (4) 이웃에 레이블이 존재하지 않으면 새로운 레이블을 부여
				else {
					label++;
					map[y * w + x] = label;
					eq_tbl[label * 2 + 0] = label;
					eq_tbl[label * 2 + 1] = label;
				}

			}
		}//w
	}//h


	 // 등가 테이블 정리----------------------
	int temp;
	for (int m = 1; m <= label; m++) {
		temp = eq_tbl[m * 2 + 1];

		if (temp != eq_tbl[m * 2 + 0])
			eq_tbl[m * 2 + 1] = eq_tbl[temp * 2 + 1];
	}

	// 등가 테이블의 레이블을 1부터 차례대로 증가시키기
	int* hash = new int[label + 1];
	memset(hash, 0, sizeof(int) * (label + 1));

	for (int n = 1; n <= label; n++)
		hash[eq_tbl[n * 2 + 1]] = eq_tbl[n * 2 + 1];

	int cnt = 1;
	for (int l = 1; l <= label; l++)
		if (hash[l] != 0)
			hash[l] = cnt++;

	for (int f = 1; f <= label; f++)
		eq_tbl[f * 2 + 1] = hash[eq_tbl[f * 2 + 1]];

	delete[] hash;

	// 두번째 스캔 시작.
	for (int p = 0; p < h; p++) {
		BYTE* pIn = imageIn.GetPtr(p);
		for (int q = 0; q < w; q++) {
			if (map[p * w + q] != 0) {

				temp = map[p * w + q];
				labelingVal = eq_tbl[temp * 2 + 1];
				pIn[q] = labelingVal;
				hist[labelingVal]++;

			}
		} // w
	}// h

	 // 제일 많은 컴포넌트를 선택한다.--------------------
	int max = 0;
	int val;
	for (int p = 0; p < 9000; p++) {
		if (hist[p] != 0 && max < hist[p]) {
			max = hist[p];
			val = p;
		}
	}

	for (int y = 0; y < h; y++) {
		BYTE* pIn = imageIn.GetPtr(y);
		BYTE* pOut = imageOut.GetPtr(y);
		for (int x = 0; x < w; x++) {
			// 픽셀수가 50이하면 지워버리자(흰색으로)
			if (hist[map[y*w + x]] < numDel) {
				pOut[x] = fabs(255 - color);
			}
			else {
				pOut[x] = color;
			}
			
		}
	}
	/*
	for (int y = 0; y<h; y++) {
		BYTE* pIn = imageIn.GetPtr(y);
		BYTE* pOut = imageOut.GetPtr(y);
		for (int x = 0; x<w; x++) {
			if (pIn[x] == val) {
				pOut[x] = color;
			}
			else {
				pOut[x] = fabs(255 - color);
			}
		}
	}

	*/

	delete[] hist;
	delete[] map;




}
void BitReversal(const CByteImage& imageIn, CByteImage& imageOut, int inObj, int inBg, int outObj, int outBg) {
	
	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	
	BYTE* pIn  = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();


	for (int r=0; r<nHeight; r++) // 영상 경계는 제외
	{
		for (int c=0; c<nWidth; c++) // 영상 경계는 제외
		{
			int index = r*nWidth +c;
			// inObj->outObj
			if (pIn[index] == inObj) 
			{
				pOut[index] = outObj;
			
			}
			// inBg->outBg
			if (pIn[index] == inBg) 
			{
				pOut[index] = outBg;
			}

		}
	}
	
}
void PupilSeg(const CByteImage& imageIn, CByteImage& imagePupil, CByteImage& imageOut) {

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pPupil = imagePupil.GetPtr();
	BYTE* pOut = imageOut.GetPtr();


	for (int r = 0; r<nHeight; r++){
		for (int c = 0; c<nWidth; c++){
			int index = r*nWidth + c;
		
			// 동공이면
			if (pPupil[index] == 0){
				pOut[index] = 0;
			}

			// 동공이 아니면
			if (pPupil[index] == 255){
				pOut[index] = pIn[index];
			}


		}
	}


}



void EyelidSeg(const CByteImage& imageIn, CByteImage& imageOut, int nThreshold)
{

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();


	for (int r = 0; r<nHeight; r++)
	{
		BYTE* pIn = imageIn.GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		for (int c = 0; c<nWidth; c++)
		{
			if (pIn[c] > nThreshold)
				pOut[c] = 255;
			else
				pOut[c] = 0;
		}
	}



}



void SobelEdge_Eyelid(const CByteImage& imageIn, CByteImage& imageOut, CByteImage& imageIris, Roi roi)
{
	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	int nWStep = imageIn.GetWStep();

	// Sobel 마스크
	int Gx[9], Gy[9], G[9];
	
	Gx[0] = -1; Gx[1] = 0; Gx[2] = 1;
	Gx[3] = -2; Gx[4] = 0; Gx[5] = 2;
	Gx[6] = -1; Gx[7] = 0; Gx[8] = 1;

	Gy[0] = 1;	Gy[1] =  2;	Gy[2] =  1;
	Gy[3] = 0;	Gy[4] =  0;	Gy[5] =  0;
	Gy[6] = -1;	Gy[7] = -2; Gy[8] = -1;

	G[0] = -1;	G[1] = -1;	G[2] = -1;
	G[3] =  2;	G[4] =  2;	G[5] =  2;
	G[6] = -1;	G[7] = -1;  G[8] = -1;

	// 가우시안 마스크
	CDoubleImage  imageGss(nWidth, nHeight);
	imageGss = _Gaussian5x5(imageIn);
	imageGss = _Gaussian5x5(imageGss);

	double* pGss = imageGss.GetPtr();

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();
	BYTE* pIris = imageIris.GetPtr();

	for (int r = roi.stY + 5; r<roi.endY - 5; r++) {
		for (int c = roi.stX + 5; c<roi.endX - 5; c++) {

			if (pIris[r*nWidth + c] == BACKGROUND) {
				int sumX = 0;
				int sumY = 0;

				for (int y = 0; y<3; y++) {
					for (int x = 0; x<3; x++) {
						int py = r - 1 + y;
						int px = c - 1 + x;

						if (px >= 0 && px<nWidth && py >= 0 && py<nHeight) {
							sumX += Gy[y * 3 + x] * pGss[py*nWStep + px];

	//						sumX += Gx[y * 3 + x] * pGss[py*nWStep + px];
	//						sumY += Gy[y * 3 + x] * pGss[py*nWStep + px];
						}
					}
				}
				if (sqrt(sumX*sumX + sumY*sumY) * 2> 40) {
					pOut[r*nWidth + c] = OBJECT;
				}
				else {
					pOut[r*nWidth + c] = BACKGROUND;
				}


			//	pOut[r*nWidth + c] = sqrt(sumX*sumX + sumY*sumY) *2;
				//	pOut[r*nWidth + c] = sqrt((double)(sumX*sumX + sumY*sumY)/32); // 경계선의 세기
				//	pOut[r*nWidth + c] = MIN(sqrt((double)(sumX*sumX + sumY*sumY) /4), 255);
			}
		}
	}
}

void CannyEdge_Eyelid(const CByteImage& imageIn, CByteImage& imageOut, CByteImage& imageIris, int nThresholdHi, int nThresholdLo, Roi roi)
{
	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nWStep = imageIn.GetWStep();

	// 1. 가우시안 마스크를 사용하여 잡음 제거
	CDoubleImage  imageGss(nWidth, nHeight);
	imageGss = _Gaussian5x5(imageIn);
	imageGss = _Gaussian5x5(imageGss);
	imageGss = _Gaussian5x5(imageGss);
	imageGss = _Gaussian5x5(imageGss);
	imageGss = _Gaussian5x5(imageGss);
	imageGss = _Gaussian5x5(imageGss);


	// Sobel 마스크
	int Gx[9], Gy[9];

	Gx[0] = -1; Gx[1] = 0; Gx[2] = 1;
	Gx[3] = -2; Gx[4] = 0; Gx[5] = 2;
	Gx[6] = -1; Gx[7] = 0; Gx[8] = 1;

	Gy[0] = 1;	Gy[1] = 2;	Gy[2] = 1;
	Gy[3] = 0;	Gy[4] = 0;	Gy[5] = 0;
	Gy[6] = -1;	Gy[7] = -2; Gy[8] = -1;
	
	CDoubleImage  imageMag(nWidth, nHeight);
	CByteImage imageAng(nWidth, nHeight);
	int nWStepG = imageGss.GetWStep();
	int nWStepA = imageAng.GetWStep();

	double* pGss = imageGss.GetPtr();

	double* pMag = imageMag.GetPtr();	// 경계선의 세기: 미분 벡터의 크기
	BYTE*   pAng = imageAng.GetPtr();	// 경계선의 방향: 미분 벡터의 방위각



	for (int r = roi.stY + 10; r<roi.endY - 10; r++){
		for (int c = roi.stX + 10; c<roi.endX - 10; c++){
			int index = r*nWidth + c;

	// 2. 소벨 마스를 이용하여 가로, 세로 방향 미분값 계산
			double sumX = 0.0;
			double sumY = 0.0;
			for (int y = 0; y<3; y++){
				for (int x = 0; x<3; x++){
					int py = r - 1 + y;
					int px = c - 1 + x;
					if (px >= 0 && px<nWidth && py >= 0 && py<nHeight){
						
						sumX += Gx[y * 3 + x] * pGss[py*nWStepG + px];
						sumY += Gy[y * 3 + x] * pGss[py*nWStepG + px];
						
					}
				}
			}


	// 3. 경계선의 세기(pMag) 및 방향(pAng)분석 (경계선 세기는 미분벡터의 크기로, 경계선의 방향은 미분 벡터의 방위각으로 계산)
			
			
			pMag[index] = sqrt(sumX*sumX + sumY*sumY); // 경계선의 세기: 미분벡터의 크기
			
			double theta;					 
			if (pMag[index] == 0){	// 경계선의 수직 방향
				if (sumY == 0){
					theta = 0;
				}
				else{
					theta = 90;
				}
			}
			else{
				theta = atan2((float)sumY, (float)sumX)*180.0 / M_PI;
			}


			if ((theta > -22.5 && theta < 22.5) || theta > 157.5 || theta < -157.5){
				pAng[index] = 0;
			}
			else if ((theta >= 22.5 && theta < 67.5) || (theta >= -157.5 && theta < -112.5)){
				pAng[index] = 45;
			}
			else if ((theta >= 67.5 && theta <= 112.5) || (theta >= -112.5 && theta <= -67.5)){
				pAng[index] = 90;
			}
			else{
				pAng[index] = 135;
			}
		} // 열 이동 끝
	} // 행 이동 끝
	

	
	// 4. 지역적을 최대 세기가 아닌 경계선 제거
	 // 비 최대값 억제
	CByteImage imageCand(nWidth, nHeight);
	imageCand.SetConstValue(0);

	for (int r = 1; r<nHeight - 1; r++){
		BYTE*	pCand = imageCand.GetPtr(r);
		double* pMag = imageMag.GetPtr(r);
		BYTE*   pAng = imageAng.GetPtr(r);
		for (int c = 1; c<nWidth - 1; c++){
			switch (pAng[c]){
			case 0:		// 0도 방향 비교
				if (pMag[c] > pMag[c - 1] && pMag[c] > pMag[c + 1]){
					pCand[c] = 255;
				}
				break;
			case 45:	// 45도 방향 비교
				if (pMag[c] > pMag[c - nWStepG + 1] && pMag[c] > pMag[c + nWStepG - 1]){
					pCand[c] = 255;
				}
				break;
			case 90:		// 90도 방향 비교
				if (pMag[c] > pMag[c - nWStepG] && pMag[c] > pMag[c + nWStepG]){
					pCand[c] = 255;
				}
				break;
			case 135:	// 135도 방향 비교
				if (pMag[c] > pMag[c - nWStepG - 1] && pMag[c] > pMag[c + nWStepG + 1]){
					pCand[c] = 255;
				}
				break;
			}
		} // 열 이동 끝
	} // 행 이동 끝

	imageCand.SaveImage("Cand.bmp");

	// 5. 경계선의 방향을 따라 문턱값 비교 수행
	imageOut = CByteImage(nWidth, nHeight);
	imageOut.SetConstValue(0);
	for (int r = 1; r<nHeight - 1; r++){
		BYTE*	pOut = imageOut.GetPtr(r);
		BYTE*	pCand = imageCand.GetPtr(r);
		double* pMag = imageMag.GetPtr(r);
		BYTE*   pAng = imageAng.GetPtr(r);
		BYTE*   pIris = imageIris.GetPtr(r);

		for (int c = 1; c<nWidth - 1; c++){
			//if(pIris[c] ==BACKGROUND){// 홍채 아닌곳 중

				if (pCand[c]) {

					if (pMag[c] > nThresholdHi) {	// 전체적인 경계선 갯수
						pOut[c] = OBJECT;
					}
					
					else if (pMag[c] > nThresholdLo){ // 연결된 픽셀 검사
						bool bIsEdge = true;
						switch (pAng[c]) {
						case 0:		// 90도 방향 검사(|)
							if ((pMag[c - nWStepG] > nThresholdLo) || (pMag[c + nWStepG] > nThresholdLo)) {
								pOut[c] = OBJECT;
							}
							break;
						case 45:	// 135도 방향 검사(\)
							if ((pMag[c - nWStepG - 1] > nThresholdLo) || (pMag[c + nWStepG + 1] > nThresholdLo)) {
								pOut[c] = OBJECT;
							}
							break;
						case 90:		// 0도 방향 검사 (-)
							if ((pMag[c - 1] > nThresholdLo) || (pMag[c + 1] > nThresholdLo)) {
									pOut[c] = OBJECT;
							}
							break;
						case 135:	// 45도 방향 검사(/)
							if ((pMag[c - nWStepG + 1] > nThresholdLo) || (pMag[c + nWStepG - 1] > nThresholdLo)) {
									pOut[c] = OBJECT;
							}
							break;
						}

					}//  if else
				}
		//	}
		} // 열 이동 끝
	} // 행 이동 끝
}


void CannyEdge_Eyelid_s(const CByteImage& imageIn, CByteImage& imageOut, CByteImage& imageIris, int nThresholdHi, int nThresholdLo, Roi roi)
{
	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nWStep = imageIn.GetWStep();

	// 1. 가우시안 마스크를 사용하여 잡음 제거
	CDoubleImage  imageGss(nWidth, nHeight);
	imageGss = _Gaussian5x5(imageIn);
	imageGss = _Gaussian5x5(imageGss);
	imageGss = _Gaussian5x5(imageGss);
	imageGss = _Gaussian5x5(imageGss);
	imageGss = _Gaussian5x5(imageGss);



	// Sobel 마스크
	int Gx[9], Gy[9];

	Gx[0] = -1; Gx[1] = 0; Gx[2] = 1;
	Gx[3] = -2; Gx[4] = 0; Gx[5] = 2;
	Gx[6] = -1; Gx[7] = 0; Gx[8] = 1;

	Gy[0] = 1;	Gy[1] = 2;	Gy[2] = 1;
	Gy[3] = 0;	Gy[4] = 0;	Gy[5] = 0;
	Gy[6] = -1;	Gy[7] = -2; Gy[8] = -1;

	CDoubleImage  imageMag(nWidth, nHeight);
	CByteImage imageAng(nWidth, nHeight);
	int nWStepG = imageGss.GetWStep();
	int nWStepA = imageAng.GetWStep();

	double* pGss = imageGss.GetPtr();

	double* pMag = imageMag.GetPtr();	// 경계선의 세기: 미분 벡터의 크기
	BYTE*   pAng = imageAng.GetPtr();	// 경계선의 방향: 미분 벡터의 방위각



	for (int r = roi.stY ; r<roi.endY ; r++) {
		for (int c = roi.stX ; c<roi.endX ; c++) {
			int index = r*nWidth + c;

			// 2. 소벨 마스를 이용하여 가로, 세로 방향 미분값 계산
			double sumX = 0.0;
			double sumY = 0.0;
			for (int y = 0; y<3; y++) {
				for (int x = 0; x<3; x++) {
					int py = r - 1 + y;
					int px = c - 1 + x;
					if (px >= 0 && px<nWidth && py >= 0 && py<nHeight) {

						sumX += Gx[y * 3 + x] * pGss[py*nWStepG + px];
						sumY += Gy[y * 3 + x] * pGss[py*nWStepG + px];

					}
				}
			}


			// 3. 경계선의 세기 및 방향 분석 (경계선 세기는 미분벡터의 크기로, 경계선의 방향은 미분 벡터의 방위각으로 계산)
			pMag[index] = sqrt(sumX*sumX + sumY*sumY); // 경계선의 세기: 미분벡터의 크기
			double theta;					 // 경계선의 수직 방향

			if (pMag[index] == 0) {
				if (sumY == 0) {
					theta = 0;
				}
				else {
					theta = 90;
				}
			}
			else {
				theta = atan2((float)sumY, (float)sumX)*180.0 / M_PI;
			}
			if ((theta > -22.5 && theta < 22.5) || theta > 157.5 || theta < -157.5) {
				pAng[index] = 0;
			}
			else if ((theta >= 22.5 && theta < 67.5) || (theta >= -157.5 && theta < -112.5)) {
				pAng[index] = 45;
			}
			else if ((theta >= 67.5 && theta <= 112.5) || (theta >= -112.5 && theta <= -67.5)) {
				pAng[index] = 90;
			}
			else {
				pAng[index] = 135;
			}
		} // 열 이동 끝
	} // 행 이동 끝



	  // 4. 지역적을 최대 세기가 아닌 경계선 제거
	  // 비 최대값 억제
	CByteImage imageCand(nWidth, nHeight);
	imageCand.SetConstValue(0);

	for (int r = 1; r<nHeight - 1; r++) {
		BYTE*	pCand = imageCand.GetPtr(r);
		double* pMag = imageMag.GetPtr(r);
		BYTE*   pAng = imageAng.GetPtr(r);
		for (int c = 1; c<nWidth - 1; c++) {
			switch (pAng[c]) {
			case 0:		// 0도 방향 비교
				if (pMag[c] > pMag[c - 1] && pMag[c] > pMag[c + 1]) {
					pCand[c] = 255;
				}
				break;
			case 45:	// 45도 방향 비교
				if (pMag[c] > pMag[c - nWStepG + 1] && pMag[c] > pMag[c + nWStepG - 1]) {
					pCand[c] = 255;
				}
				break;
			case 90:		// 90도 방향 비교
				if (pMag[c] > pMag[c - nWStepG] && pMag[c] > pMag[c + nWStepG]) {
					pCand[c] = 255;
				}
				break;
			case 135:	// 135도 방향 비교
				if (pMag[c] > pMag[c - nWStepG - 1] && pMag[c] > pMag[c + nWStepG + 1]) {
					pCand[c] = 255;
				}
				break;
			}
		} // 열 이동 끝
	} // 행 이동 끝

	imageCand.SaveImage("Cand.bmp");

	// 5. 경계선의 방향을 따라 문턱값 비교 수행
	imageOut = CByteImage(nWidth, nHeight);
	imageOut.SetConstValue(0);
	for (int r = 1; r<nHeight - 1; r++) {
		BYTE*	pOut = imageOut.GetPtr(r);
		BYTE*	pCand = imageCand.GetPtr(r);
		double* pMag = imageMag.GetPtr(r);
		BYTE*   pAng = imageAng.GetPtr(r);
		BYTE*   pIris = imageIris.GetPtr(r);

		for (int c = 1; c<nWidth - 1; c++) {
			if (pIris[c] == BACKGROUND) {// 홍채 아닌곳 중

				if (pCand[c]) {

					if (pMag[c] > nThresholdHi) {	// 전체적인 경계선 갯수
						pOut[c] = OBJECT;
					}

					else if (pMag[c] > nThresholdLo) { // 연결된 픽셀 검사
						bool bIsEdge = true;
						switch (pAng[c]) {
						case 0:		// 90도 방향 검사(|)
							if ((pMag[c - nWStepG] > nThresholdLo) || (pMag[c + nWStepG] > nThresholdLo)) {
								pOut[c] = OBJECT;
							}
							break;
						case 45:	// 135도 방향 검사(\)
							if ((pMag[c - nWStepG - 1] > nThresholdLo) || (pMag[c + nWStepG + 1] > nThresholdLo)) {
								pOut[c] = OBJECT;
							}
							break;
						case 90:		// 0도 방향 검사 (-)
							if ((pMag[c - 1] > nThresholdLo) || (pMag[c + 1] > nThresholdLo)) {
									pOut[c] = OBJECT;
							}
							break;
						case 135:	// 45도 방향 검사(/)
							if ((pMag[c - nWStepG + 1] > nThresholdLo) || (pMag[c + nWStepG - 1] > nThresholdLo)) {
									pOut[c] = OBJECT;
							}
							break;
						}

					}//  if else
				}
			}
		} // 열 이동 끝
	} // 행 이동 끝
}