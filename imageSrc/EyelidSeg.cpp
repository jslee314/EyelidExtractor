#include "stdafx.h"
#include "EyelidSeg.h"
#include "Features.h"

#include <math.h>
#include <algorithm>

void AdaptiveBinarization (const CByteImage& imageIn, CByteImage& imageOut, CByteImage& imageIris)
{
	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();
	BYTE* pIris = imageIris.GetPtr();

	register int i, j, k, l;
	int gval, index1, index2;
	float mean, vari, thres;
	int W = 5;

	for (i = 0; i < nHeight*nWidth; i++) {
		pOut[i] = BACKGROUND;
	}

	for (i = 0; i<nHeight; i++){
		index2 = i*nWidth;
		for (j = 0; j<nWidth; j++){
			if (pIris[index2 + j] == BACKGROUND) {
				float gsum = 0.0f;
				float ssum = 0.0f;
				int   count = 0;

				for (k = i - W; k <= i + W; k++) {
					index1 = k*nWidth;
					if (k<0 || k >= nHeight) continue;

					for (l = j - W; l <= j + W; l++) {
						if (l<0 || l >= nWidth) continue;

						gval = pIn[index1 + l];
						gsum += gval;
						ssum += gval*gval;
						count++;
					}
				}

				mean = gsum / (float)count;
				vari = ssum / (float)count - mean*mean;

				if (vari<0) vari = 0.0f;

				//			thres	= mean+0.4f*(float)sqrt(vari);
				thres = mean*(1.0f - 0.02f*(1 - (float)sqrt(vari) / 128));

				if (pIn[index2 + j]<thres) {
					pOut[index2 + j] = OBJECT;
				}

			}

		}
	}
}
void Otzu_Threshold(const CByteImage& imageIn, CByteImage& imageOut){
	
	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();

	register int i, t;

	int grayscal = 256;	//256

	// Histogram set----------------
	int		hist[256];
	float   prob[256];

	for (i = 0; i<grayscal; i++) {
		hist[i] = 0; 
		prob[i] = 0.0f; 
	}
	for (int r = 1; r<nHeight-1; r++) {
		for (int c = 1; c < nWidth-1; c++) {
			int index = r*nWidth + c;
			hist[(int)pIn[index]]++;
		}
	}
	for (i = 0; i < grayscal; i++) {
		prob[i] = (float)hist[i] / (float)(nHeight*nWidth);
	}

	//FILE *f = fopen("D:\hist.txt", "a");
	//for (int i = 0; i < grayscal; i++) {
	//	fprintf(f, "%d\n", hist[i]);
	//}
	//fclose(f);
		

	float wsv_min = 1000000.0f;
	float wsv_u1, wsv_u2, wsv_s1, wsv_s2;
	int wsv_t;

	for (t = 60; t<200; t++){
		// q1, q2 계산 
		float q1 = 0.0f, q2 = 0.0f;

		for (i = 0; i < t; i++) {
			q1 += prob[i];
		}

		for (i = t; i < grayscal; i++) {
			q2 += prob[i];
		}
			
		if (q1 == 0 || q2 == 0) 
			continue;

		// u1, u2 계산 
		float u1 = 0.0f, u2 = 0.0f;
		for (i = 0; i < t; i++) {
			u1 += i*prob[i];
			u1 /= q1;
		}
		for (i = t; i < grayscal; i++) {
			u2 += i*prob[i];
			u2 /= q2;
		}

		// s1, s2 계산 
		float s1 = 0.0f, s2 = 0.0f;
		for (i = 0; i < t; i++) {
			s1 += (i - u1)*(i - u1)*prob[i]; 
			s1 /= q1;
		}
		for (i = t; i < grayscal; i++) {
			s2 += (i - u2)*(i - u2)*prob[i]; 
			s2 /= q2;
		}

		float wsv = q1*s1 + q2*s2;

		if (wsv < wsv_min){
			wsv_min = wsv;	wsv_t = t;
			wsv_u1 = u1; 	wsv_u2 = u2;
			wsv_s1 = s1; 	wsv_s2 = s2;
		}
	}

	// thresholding
	for (int r = 1; r<nHeight - 1; r++) {
		for (int c = 1; c < nWidth - 1; c++) {
			int index = r*nWidth + c;
			if (pIn[index] < wsv_t) {
				pOut[index] = BACKGROUND;
			}
			else {
				pOut[index] = OBJECT;
			}
		}
	}
}

void Stepwise_Threshold(const CByteImage& imageIn, CByteImage& imageOut)
{
	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();

	for (int r = 0; r < nHeight; r++) {
		for (int c = 0; c < nWidth; c++) {
			int index = r*nWidth + c;
			if (pIn[index] <= 40) {
				pOut[index] = 0;
			}
			else if (40 < pIn[index] && pIn[index]<= 60) {
				pOut[index] = 40;
			}
			else if (60 < pIn[index] &&  pIn[index] <= 80) {
				pOut[index] = 60;
			}
			else if (80 < pIn[index] && pIn[index] <= 100) {
				pOut[index] = 80;
			}
			else if (100 < pIn[index] && pIn[index] <= 120) {
				pOut[index] = 100;
			}
			else if (120 < pIn[index] && pIn[index] <= 140) {
				pOut[index] = 120;
			}
			else if (140 < pIn[index] && pIn[index] <= 160) {
				pOut[index] = 140;
			}
			else if (160 < pIn[index] && pIn[index] <= 180) {
				pOut[index] = 160;
			}
			else if (180 < pIn[index] && pIn[index]  <= 200) {
				pOut[index] = 180;
			}
			else if(180 < pIn[index] && pIn[index]  <= 255) {
				pOut[index] = 255;
			}

		}
	}
}


void NonMaxSuppression(const CByteImage& imageIn, CByteImage& imageOut) {

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	BYTE*	pIn = imageIn.GetPtr();
	BYTE*	pOut = imageOut.GetPtr();


}
/*
void NonMaxSuppression(const CByteImage& imageIn, CByteImage& imageOut) {
	

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	
	CByteImage imageCand(nWidth, nHeight);

	CDoubleImage imageMag(nWidth, nHeight);
	CByteImage	 imageAng(nWidth, nHeight);


	CDoubleImage  imageGss(nWidth, nHeight);
	imageGss = _Gaussian5x5(imageIn);

	double* pGss = imageGss.GetPtr();

	double* pMag = imageMag.GetPtr();	// 경계선의 세기: 미분 벡터의 크기
	BYTE*   pAng = imageAng.GetPtr();	// 경계선의 방향: 미분 벡터의 방위각

	BYTE*	pIn = imageIn.GetPtr();
	BYTE*	pOut = imageOut.GetPtr();

	int Gx[9], Gy[9];

	Gx[0] = -1; Gx[1] = 0; Gx[2] = 1;
	Gx[3] = -2; Gx[4] = 0; Gx[5] = 2;
	Gx[6] = -1; Gx[7] = 0; Gx[8] = 1;

	Gy[0] = 1;	Gy[1] = 2;	Gy[2] = 1;
	Gy[3] = 0;	Gy[4] = 0;	Gy[5] = 0;
	Gy[6] = -1;	Gy[7] = -2; Gy[8] = -1;

	imageCand.SetConstValue(0);


	for (int r = 1; r < nHeight - 1; r++) {
		for (int c = 1; c < nWidth - 1; c++) {
			int index = r*nWidth + c;

			// 2. 소벨 마스를 이용하여 가로, 세로 방향 미분값 계산
			double sumX = 0.0;
			double sumY = 0.0;
			for (int y = 0; y<3; y++) {
				for (int x = 0; x<3; x++) {
					int py = r - 1 + y;
					int px = c - 1 + x;
					if (px >= 0 && px<nWidth && py >= 0 && py<nHeight) {

						sumX += Gx[y * 3 + x] * pGss[py*nWidth + px];
						sumY += Gy[y * 3 + x] * pGss[py*nWidth + px];

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
	// 비 최대값 억제 Non-maximum suppression


	for (int r = 1; r < nHeight - 1; r++) {
		for (int c = 1; c < nWidth - 1; c++) {
			int index = r*nWidth + c;

			switch (pAng[index]) {
			case 0:		// 0도 방향 비교
				if (pMag[index] > pMag[index -1] && pMag[index] > pMag[index + 1]) {
					pOut[index] = OBJECT;
				}
				break;
			case 45:	// 45도 방향 비교
				if (pMag[index] > pMag[index - nWidth + 1] && pMag[index] > pMag[index + nWidth - 1]) {
					pOut[index] = OBJECT;
				}
				break;
			case 90:		// 90도 방향 비교
				if (pMag[index] > pMag[index - nWidth] && pMag[index] > pMag[index + nWidth]) {
					pOut[index] = OBJECT;
				}
				break;
			case 135:	// 135도 방향 비교
				if (pMag[index] > pMag[index - nWidth - 1] && pMag[index] > pMag[index + nWidth + 1]) {
					pOut[index] = OBJECT;
				}
				break;
			}
		}	// 열 이동 끝
	}	// 행 이동 끝
}
*/



// Get Boundary
void ContourExtraction(const CByteImage& imageIn, CByteImage& imageOut)
{
	typedef struct tagBORDERINFO { short *x, *y; short n, dn; } BORDERINFO;
	BORDERINFO stBorderInfo[1000];

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();


	short *xchain = new short[100000];
	short *ychain = new short[100000];

	//clockwise neighbors
	const POINT nei[8] = { { 1,0 },{ 1,-1 },{ 0,-1 },{ -1,-1 },{ -1,0 },{ -1,1 },{ 0,1 },{ 1,1 } };

	int numberBorder = 0;
	int border_count, diagonal_count;
	unsigned char c0, c1;
	int x0, y0, x, y, n, k;


	for (y = 1; y < nHeight; y++){
		for (x = 1; x < nWidth; x++){

			c0 = pIn[y*nWidth + x];
			c1 = pIn[(y - 1)*nWidth + x];

			//contour 라면
			if (c0 != c1 && c0 == OBJECT && pOut[y*nWidth + x] == BACKGROUND){
				border_count = 0;
				diagonal_count = 0;
				x0 = x;
				y0 = y;
				n = 4;//관심점 주위의 4번 점에서 경계추적 시작
				do {
					for (k = 0; k < 8; k++, n = ((n + 1) & 7))
					{
						int u = (int)(x + nei[n].x);
						int v = (int)(y + nei[n].y);
						if (u < 0 || u >= nWidth || v < 0 || v >= nHeight)
							continue;
						else if (pIn[v*nWidth + u] == OBJECT)
							break;
					}
					//고립점
					if (k == 8)
						break;
					pOut[y*nWidth + x] = OBJECT;
					xchain[border_count] = x;
					ychain[border_count++] = y;
					if (border_count >= 10000)
						break;

					x = x + nei[n].x;
					y = y + nei[n].y;

					if (n % 2 == 1)
						diagonal_count++;

					n = (n + 5) & 7;
				} while (!(x == x0 && y == y0));

				if (k == 8)
					continue;
			}
		}
	}
}
void Contour_fitting(const CByteImage& imageIn, CByteImage& imageOut) {
	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();

	int n = 0;
	for (int r = 0; r < nHeight; r++) {
		for (int c = 0; c < nWidth; c++) {
			if (pIn[r*nWidth + c] == OBJECT) {
				n++;
			}
		}
	}
	printf("number of contour point :%d \n", n);

	point p[10000];
	for (int i = 0; i<10000; i++) {
		p[i].x = 0;
		p[i].y = 0;
		p[i].seq = 0;
	}
	point q[10000];

	point basket[10000];

	int min_index = 1;		// 제일 아래 오른쪽 점: (p[min_index].x, p[min_index].y)
	int i = 1;
	//제일 아래 오른쪽 점 저장
	for (int r = 0; r < nHeight; r++) {
		for (int c = 0; c < nWidth; c++) {
			if (pIn[r*nWidth + c] == OBJECT) {
				p[i].x = c;
				p[i].y = r;
				p[i].seq = i;

				//printf("[%d,%d,%d] \n",p[i].x, p[i].y, p[i].seq);
				if (p[min_index].y > p[i].y || p[min_index].y == p[i].y && p[min_index].x <= p[i].x) {
					min_index = i;
				}
				i++;
			}
		}
	}
	//-------------------------------------
	int cnt;
	point tmp;
	// p[1]과 p[min_index]를 바꿈
	tmp = p[min_index];
	p[min_index] = p[1];
	p[1] = tmp;
	// printf("min_index: %d \n", min_index);

	int cnt_extreme_point = 1;

	// march up
	while (1) {
		cnt = 0;
		// 1. 이 점으로 축을 옮긴 후 
		point tmp1;
		tmp1 = p[cnt_extreme_point];
		for (int i = cnt_extreme_point + 1; i <= n; i++) {
			q[i].x = p[i].x - tmp1.x;
			q[i].y = p[i].y - tmp1.y;
			q[i].seq = p[i].seq;
		}


		// 2. y 값이 양수이거나 0 인 값을 baskey에 모아서
		for (i = cnt_extreme_point + 1; i <= n; i++) {
			if (q[i].y >= 0) {
				basket[++cnt] = q[i];
			}
		}

		if (cnt == 0) {
			break;
		}
		// 3. 그 점 중에 외적을 이용하여 가장 각도가 작은 점을 구함 
		min_index = 1;
		for (i = 2; i <= cnt; i++) {
			if (cross_product(basket[min_index], basket[i]) < 0) {
				min_index = i;
			}
			else if (cross_product(basket[min_index], basket[i]) == 0 && distance(basket[min_index]) < distance(basket[i])) {
				min_index = i;
			}
		}
		cnt_extreme_point++;

		// 4. 이 점을 원점을 원점으로 놓고 2 -3 번 과정을 반복 
		for (i = cnt_extreme_point + 1; i <= n; i++) {
			if (p[i].seq == basket[min_index].seq) {
			//	printf("up basket[min_index].seq: %d \n", basket[min_index].seq);
				break;
			}
		}

		tmp = p[i];
		p[i] = p[cnt_extreme_point];
		p[cnt_extreme_point] = tmp;
	}


	//march down
	while (1) {
		cnt = 0;
		// 1. y 값이 음수이거나 0 인 값을 모아서 
		for (i = cnt_extreme_point + 1; i <= n; i++) {
			if (q[i].y <= 0) {
				basket[++cnt] = q[i];
			}
		}

		if (cnt == 0) {
			//return; //종료
			break;
		}

		// 2. 그 점 중에 외적을 이용하여 가장 각도가 작은 점을 구함 
		min_index = 1;
		for (i = 2; i <= cnt; i++) {
			if (cross_product(basket[min_index], basket[i]) < 0) {
				min_index = i;
			}
			else if (cross_product(basket[min_index], basket[i]) == 0 && distance(basket[min_index]) < distance(basket[i]))
			{
				min_index = i;
			}
		}
		tmp.x = p[1].x - p[cnt_extreme_point].x;
		tmp.y = p[1].y - p[cnt_extreme_point].y;

		if (cross_product(tmp, basket[min_index]) > 0) {
			//return; //종료 
			break;
		}
		for (i = cnt_extreme_point + 1; i <= n; i++) {
			if (p[i].seq == basket[min_index].seq) {
				// printf("down basket[min_index].seq: %d \n",basket[min_index].seq);
				break;
			}
		}

		cnt_extreme_point++;

		tmp = p[i];
		p[i] = p[cnt_extreme_point];
		p[cnt_extreme_point] = tmp;

		//recoordinte(n,cnt_extreme_point, p, q);
		point tmp2;
		tmp2 = p[cnt_extreme_point];
		for (int i = cnt_extreme_point + 1; i <= n; i++) {
			q[i].x = p[i].x - tmp2.x;
			q[i].y = p[i].y - tmp2.y;
			q[i].seq = p[i].seq;
		}
	}
	//printf("cnt_extreme_point: %d \n", cnt_extreme_point);


	//-------------------------------------------------------------------------------------------
	for (int i = 1; i <= cnt_extreme_point; i++) {
		int x1 = p[i].x;
		int y1 = p[i].y;
		pOut[y1*nWidth + x1] = OBJECT;

		int x2;
		int y2;

		if (i<cnt_extreme_point) {
			x2 = p[i + 1].x;
			y2 = p[i + 1].y;;
		}
		else {//(i==cnt_extreme_point)
			x2 = p[1].x;
			y2 = p[1].y;
		}
		//---------------------------------------------------

		if (x2 == x1) {
			int xx = x1;
			int yy = y2;
			pOut[yy * nWidth + xx] = OBJECT;
			// printf("x1 %d,%d  \n", xx, yy);
		}
		if (x1 > x2) {
			for (int xx = x1; xx >= x2; xx--) {
				int yy = (int)(((double)(y2 - y1) / (double)(x2 - x1)*(xx - x1)) + y1);
				pOut[yy * nWidth + xx] = OBJECT;
				// printf("x2 %d,%d  \n", xx, yy);
			}
		}
		if (x1 < x2) {
			for (int xx = x1; xx <= x2; xx++) {
				int yy = (int)(((double)(y2 - y1) / (double)(x2 - x1)*(xx - x1)) + y1);
				pOut[yy *nWidth + xx] = OBJECT;
				// printf("x3 %d,%d  \n", xx, yy);
			}
		}

		//---------------------------------------------------
		if (y2 == y1) {
			int yy = y1;
			int xx = x2;
			pOut[yy *nWidth + xx] = OBJECT;
		}
		else if (y1>y2) {
			for (int yy = y1; yy >= y2; yy--) {
				int xx = (int)((double)((yy - y1) / (double)(y2 - y1)*(x2 - x1)) + x1);
				pOut[yy *nWidth + xx] = OBJECT;
			}

		}
		else if (y1<y2) {
			for (int yy = y1; yy <= y2; yy++) {
				int xx = (int)((double)((yy - y1) / (double)(y2 - y1)*(x2 - x1)) + x1);
				pOut[yy *nWidth + xx] = OBJECT;
			}
		}
	}

}



// 제일 아래 오른쪽 점을 선택 후 이 점에서 고무줄을 줄게 늘여서 위로 올리면 제일 먼저 만나는 점이 convex hull 에 포함되므로 이를 반복하여 구하는 방법이다. 
void convex_hull(const CByteImage& imageIn, CByteImage& imageOut) {

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	BYTE* pIn = imageIn.GetPtr();
	BYTE* pOut = imageOut.GetPtr();

	int n = 0;
	for (int r = 0; r < nHeight; r++) {
		for (int c = 0; c < nWidth; c++) {
			if (pIn[r*nWidth + c] == OBJECT) {
				n++;
			}
		}
	}
//	printf("number of contour point :%d \n", n);

	point p[10000];
	for (int i = 0; i<10000; i++) {
		p[i].x = 0;
		p[i].y = 0;
		p[i].seq = 0;
	}
	point q[10000];

	point basket[10000];

	int min_index = 1;		// 제일 아래 오른쪽 점: (p[min_index].x, p[min_index].y)
	int i = 1;
	//제일 아래 오른쪽 점 저장
	for (int r = 0; r < nHeight; r++){
		for (int c = 0; c < nWidth; c++){
			if (pIn[r*nWidth + c] == OBJECT){
				p[i].x = c;
				p[i].y = r;
				p[i].seq = i;

				//printf("[%d,%d,%d] \n",p[i].x, p[i].y, p[i].seq);
				if (p[min_index].y > p[i].y || p[min_index].y == p[i].y && p[min_index].x <= p[i].x) {
					min_index = i;
				}
				i++;
			}
		}
	}
	//-------------------------------------
	int cnt;
	point tmp;
	// p[1]과 p[min_index]를 바꿈
	tmp = p[min_index];
	p[min_index] = p[1];
	p[1] = tmp;
	//printf("min_index: %d \n",min_index);

	int cnt_extreme_point = 1;

	//march up
	while (1) {
		cnt = 0;
		// 1. 이 점으로 축을 옮긴 후 
		point tmp1;
		tmp1 = p[cnt_extreme_point];
		for (int i = cnt_extreme_point + 1; i <= n; i++) {
			q[i].x = p[i].x - tmp1.x;
			q[i].y = p[i].y - tmp1.y;
			q[i].seq = p[i].seq;
		}


		// 2. y 값이 양수이거나 0 인 값을 baskey에 모아서
		for (i = cnt_extreme_point + 1; i <= n; i++) {
			if (q[i].y >= 0) {
				basket[++cnt] = q[i];
			}
		}

		if (cnt == 0) {
			break;
		}
		// 3. 그 점 중에 외적을 이용하여 가장 각도가 작은 점을 구함 
		min_index = 1;
		for (i = 2; i <= cnt; i++) {
			if (cross_product(basket[min_index], basket[i]) < 0) {
				min_index = i;
			}
			else if (cross_product(basket[min_index], basket[i]) == 0 && distance(basket[min_index]) < distance(basket[i])) {
				min_index = i;
			}
		}
		cnt_extreme_point++;

		// 4. 이 점을 원점을 원점으로 놓고 2 -3 번 과정을 반복 
		for (i = cnt_extreme_point + 1; i <= n; i++) {
			if (p[i].seq == basket[min_index].seq) {
			//	printf("up basket[min_index].seq: %d \n", basket[min_index].seq);
				break;
			}
		}

		tmp = p[i];
		p[i] = p[cnt_extreme_point];
		p[cnt_extreme_point] = tmp;
	}


	//march down
	while (1) {
		cnt = 0;
		// 1. y 값이 음수이거나 0 인 값을 모아서 
		for (i = cnt_extreme_point + 1; i <= n; i++) {
			if (q[i].y <= 0) {
				basket[++cnt] = q[i];
			}
		}

		if (cnt == 0) {
			//return; //종료
			break;
		}

		// 2. 그 점 중에 외적을 이용하여 가장 각도가 작은 점을 구함 
		min_index = 1;
		for (i = 2; i <= cnt; i++) {
			if (cross_product(basket[min_index], basket[i]) < 0) {
				min_index = i;
			}
			else if (cross_product(basket[min_index], basket[i]) == 0 && distance(basket[min_index]) < distance(basket[i]))
			{
				min_index = i;
			}
		}
		tmp.x = p[1].x - p[cnt_extreme_point].x;
		tmp.y = p[1].y - p[cnt_extreme_point].y;

		if (cross_product(tmp, basket[min_index]) > 0) {
			//return; //종료 
			break;
		}
		for (i = cnt_extreme_point + 1; i <= n; i++) {
			if (p[i].seq == basket[min_index].seq) {
				// 				printf("down basket[min_index].seq: %d \n",basket[min_index].seq);
				break;
			}
		}

		cnt_extreme_point++;

		tmp = p[i];
		p[i] = p[cnt_extreme_point];
		p[cnt_extreme_point] = tmp;

		//recoordinte(n,cnt_extreme_point, p, q);
		point tmp2;
		tmp2 = p[cnt_extreme_point];
		for (int i = cnt_extreme_point + 1; i <= n; i++) {
			q[i].x = p[i].x - tmp2.x;
			q[i].y = p[i].y - tmp2.y;
			q[i].seq = p[i].seq;
		}
	}
//	printf("cnt_extreme_point: %d \n",cnt_extreme_point);


	//-------------------------------------------------------------------------------------------
	for (int i = 1; i <= cnt_extreme_point; i++) {
		int x1 = p[i].x;
		int y1 = p[i].y;

		pOut[y1*nWidth+x1] = OBJECT;

		// printf("x1 %d,%d  \n", p[i].x, p[i].y);
		int x2;
		int y2;

		if (i<cnt_extreme_point) {
			x2 = p[i + 1].x;
			y2 = p[i + 1].y;;
		}
		else {//(i==cnt_extreme_point)
			x2 = p[1].x;
			y2 = p[1].y;
		}
		//---------------------------------------------------

		if (x2 == x1) {
			int xx = x1;
			int yy = y2;
			pOut[yy * nWidth + xx] = OBJECT;
			// printf("x1 %d,%d  \n", xx, yy);
		}
		if (x1 > x2) {
			for (int xx = x1; xx >= x2; xx--) {
				int yy = (int)(((double)(y2 - y1) / (double)(x2 - x1)*(xx - x1)) + y1);
				pOut[yy * nWidth + xx] = OBJECT;
				// printf("x2 %d,%d  \n", xx, yy);
			}
		}
		if (x1 < x2) {
			for (int xx = x1; xx <= x2; xx++) {
				int yy = (int)(((double)(y2 - y1) / (double)(x2 - x1)*(xx - x1)) + y1);
				pOut[yy *nWidth + xx] = OBJECT;
				// printf("x3 %d,%d  \n", xx, yy);
			}
		}

		//---------------------------------------------------
		if (y2 == y1) {
			int yy = y1;
			int xx = x2;
			pOut[yy *nWidth + xx] = OBJECT;
			// printf("y1 %d,%d  \n", xx, yy);
		}
		else if (y1>y2) {
			for (int yy = y1; yy >= y2; yy--) {
				int xx = (int)((double)((yy - y1) / (double)(y2 - y1)*(x2 - x1)) + x1);
				pOut[yy *nWidth + xx] = OBJECT;
				// printf("y2 %d,%d  \n", xx, yy);
			}

		}
		else if (y1<y2) {
			for (int yy = y1; yy <= y2; yy++) {
				int xx = (int)((double)((yy - y1) / (double)(y2 - y1)*(x2 - x1)) + x1);
				pOut[yy *nWidth + xx] = OBJECT;
				// printf("y3 %d,%d  \n", xx, yy);
			}
		}
	}
	
}


// 거리
double distance(point tmp)
{
	double sq = (double)(sqrt((double)(tmp.x*tmp.x + tmp.y*tmp.y)));

	return sq;
}
// 내적
int cross_product(point m, point n)
{
	int prod = (int)(m.x*n.y - m.y*n.x);

	return prod;
}

Roi Detect_ROI( CByteImage& imageObj, CByteImage& imageOut, const CByteImage& imageIn, Roi rRoi){

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();


	BYTE* pObj = imageObj.GetPtr();

	for (int r = 1; r < nHeight-1; r++) {
		for (int c = 1; c < nWidth-1; c++) {
			if (pObj[r*nWidth + c] == OBJECT) {

				// start(0,0에서 object 만나면 해당 값 저장)
				if (rRoi.stY > r) {
					rRoi.stY = r;
				}
				if (rRoi.stX > c) {
					rRoi.stX = c;
				}

				if (rRoi.endY < r) {
					rRoi.endY = r;
				}
				if (rRoi.endX < c) {
					rRoi.endX = c;
				}

			}
		}
	}

	// printf("(%d,%d) (%d,%d)\n", rRoi.stX, rRoi.stY, rRoi.endX, rRoi.endY);
	return rRoi;

}

void Crop_ROI(const CByteImage& imageIn, CByteImage& imageOut,  Roi Rroi) {

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	BYTE *pIn = imageIn.GetPtr();
	BYTE *pOut = imageOut.GetPtr();

	for (int r = Rroi.stY; r<Rroi.endY; r++) {
		for (int c = Rroi.stX; c<Rroi.endX; c++) {
			int index = r*nWidth + c;
				pOut[index] = pIn[index];
		}
	}
}
/*
class RegionGrowingCondition
{
public:
	virtual bool IsGrowable(BYTE srcByte, BYTE dstByte)
	{
		return (srcByte == BITMAPIMAGE && dstByte != OBJECT && dstByte != MASKFENCE);
	};

	virtual BYTE GetGrowingValue()
	{
		return OBJECT;
	}
};

class ReverseRegionGrowingCondition : public RegionGrowingCondition
{
public:
	virtual bool IsGrowable(BYTE srcByte, BYTE dstByte)
	{
		return (srcByte == OBJECT && dstByte != MASKFENCE && dstByte != BACKGROUND);
	};

	virtual BYTE GetGrowingValue()
	{
		return BACKGROUND;
	}
};


void DoRegionGrowing(RegionGrowingCondition* condition, int seedX, int seedY)
{
	int w = _width, h = _height;
	int index = w * seedY + seedX;
	_condition = condition;

	if (seedX < 0 || seedX >= w || seedY < 0 || seedY >= h)
		return;

	if (!condition->IsGrowable(_src[index], _dst[index]))
	{
		return;
	}
	_Q.push(index);

	do {
		index = _Q.front();
		_Q.pop();

		int xPos = index % w,
			yPos = index / w;
		int	yIndex = w * yPos;

		bool topConnectivity = false;
		bool bottomConnectivity = false;
		bool topCheck = true;
		bool bottomCheck = true;

		// if top line or bottom line of slice 
		if (yPos == 0) topCheck = false;
		if (yPos == h - 1) bottomCheck = false;

		bool isLineGrowable = true;

		// to left side from seed
		for (int x = xPos; x >= 0 && isLineGrowable; x--)
		{
			int newIndex = yIndex + x;
			if (!_condition->IsGrowable(_src[newIndex], _dst[newIndex]))
			{
				isLineGrowable = false;
			}
			else {
				_dst[x + yIndex] = _condition->GetGrowingValue();

				if (topCheck)
				{
					// above pixel check
					int upperIndex = newIndex - w;
					topConnectivity = SowSeed(upperIndex, topConnectivity);
				}

				if (bottomCheck)
				{
					// below pixel check
					int lowerIndex = newIndex + w;
					bottomConnectivity = SowSeed(lowerIndex, bottomConnectivity);
				}
			} // else
		} // for(right)

		isLineGrowable = true;

		// to right side from seed
		for (int x = xPos + 1; x < w && isLineGrowable; x++)
		{
			int newIndex = yIndex + x;
			if (!_condition->IsGrowable(_src[newIndex], _dst[newIndex]))
			{
				isLineGrowable = false;
			}
			else {
				_dst[x + yIndex] = _condition->GetGrowingValue();

				if (topCheck)
				{
					// above pixel check
					int upperIndex = newIndex - w;
					topConnectivity = SowSeed(upperIndex, topConnectivity);
				}

				if (bottomCheck)
				{
					// below pixel check
					int lowerIndex = newIndex + w;
					bottomConnectivity = SowSeed(lowerIndex, bottomConnectivity);
				}
			} // else
		} // for(right)
	} while (!_Q.empty());
}*/

void ToPixel(const CByteImage& imageIn, CByteImage& imageOut) {
	
	int nWidth  = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	int *imageLabel = new int[1200];				// UINT: unsigned int
	memset(imageLabel, 0, sizeof(int) * 1200);

	FILE *f = fopen("D:\device1_0050_001.txt", "r");
	for (int i = 0; i < 1200; i++) {
		int a;
		fscanf(f, "%d\n",&a);
		imageLabel[i] = a;
	}
	fclose(f);

	BYTE *pIn = imageIn.GetPtr();
	BYTE *pOut = imageOut.GetPtr();

	for (int r = 0; r < 30; r++) {
		for (int c = 0; c < 40; c++) {
			int index = r * 40 + c;

			if (imageLabel[index] == 1) {

				for (int rr = 0; rr < 16; rr++) {
					for (int cc = 0; cc < 16; cc++) {

						int r16 = (r * 16) + rr /*- 8*/;
						int c16 = (c * 16) + cc /*- 8*/;
						int index2 = r16 * nWidth + c16;

						//pOut[index2] = OBJECT;
						 pOut[index2] = pIn[index2];
					}
				}
			}
			else {
				pOut[index] = BACKGROUND;

			}
		}

	}

}


void HoughParabolaDetction(CByteImage& imageObj, CByteImage& imageOut, parabolaComponent parabolaCompo){

	int nWidth = imageObj.GetWidth();
	int nHeight = imageObj.GetHeight();
	int nWStep = imageObj.GetWStep();

	int sliceSize = nWidth*nHeight;

	BYTE* pObj = imageObj.GetPtr();
	//BYTE* pOut = imageOut.GetPtr();
	
	// x0: 280, y0:120  a:180 o:0
	int x0, y0, a, o;	// 포물선의 중심점(x0,y0), 준선의 방정식계수 a, 타원의 기울기 o
	
	int x, y;			// 포물선 위의 한 점 좌표
	double max = 0;

	int votingThreshold = 50;
	int num_ang = 360;

	int* pCntTrans = new int[sliceSize];
	memset(pCntTrans, 0, sliceSize * sizeof(int));

	double* tsin = new double[360];		// 0~360도의 sin,cos값 lookup table
	double* tcos = new double[360];

	for (int theta = 0; theta<num_ang; theta++) {
		tsin[theta] = (double)sin((double)theta*M_PI / (double)180);
		tcos[theta] = (double)cos((double)theta*M_PI / (double)180);
	}

	
	vector <EHTelmt> paramVoting; //축적 배열 역할
	paramVoting.clear();

	double mt = 0.0;				// 타원 매개변수 방정식의 매개변수t
	mt = (double)(mt - 0.8080);

	int xx = parabolaCompo.x0;
	int yy = parabolaCompo.y0;
	int aa = parabolaCompo.a;
	int aa1, aa2;
	if (aa == 150) {
		aa1 = 50;
		aa2 = 300;
	}
	else {
		aa1 = -50;
		aa2 = -450;
	}

	for (a = aa1; a< aa2; a += 2) {		// 준선계수

		for (o = -10; o<10; o ++) {				// Rotation factor: o
			

			for (y0 = yy-50; y0<yy+50; y0++) {	 // translation factor: (x0,y0)
				for (x0 = xx-50; x0<xx+50; x0++) {
//			for (y0 = 80; y0<200; y0++) {	 // translation factor: (x0,y0)
//				for (x0 = 200; x0<350; x0++) {



					if (pObj[y0 * nWidth + x0] == OBJECT) {	// 에지 픽셀 중에서
						for (int cnt = 0; cnt<960; cnt++) {

							x = x0 + (int)(2 * a * mt * tcos[o]) + (int)(a * mt*mt* tsin[o]);			// 포물선 위의 한 점(x,y)
							y = y0 - (int)(2 * a * mt * tsin[o]) + (int)(a * mt*mt* tcos[o]);
							mt += 0.0025;
							if ((x >= 0 && x < nWidth) && (y >= 0 && y < nHeight)) {	// 포물선 위의 한 점이 영상 범위 내에 있을 때
								pCntTrans[y * nWidth + x]++;
							}
						}
					}
				}
			}
			// 축적 vector에 타원에 따른 축적 정보 넣음
			for (y0 = 0; y0<nHeight; y0++) {	 // translation factor: (x0,y0)
				for (x0 = 0; x0<nWidth; x0++) {

					if (pCntTrans[y0 * nWidth + x0] > votingThreshold) {

						EHTelmt e(pCntTrans[y0 * nWidth + x0], x0, y0, a, o); // 중심이 (x0,y0), 장축길이 l, 단축길이 s, 기울기 o인 타원
						paramVoting.push_back(e); // 축적값 vector에 저장
						


					}
				}
			}
		}
	}

	delete[] tsin;
	delete[] tcos;

	sort(paramVoting.begin(), paramVoting.end());		// 축적vector를 오름차순으로 정렬
	reverse(paramVoting.begin(), paramVoting.end());	// 역 정렬하여 내림차순으로 만듦

														// 축적 값 상위 0.001%의 타원만 그림
	for (x = 0; x<paramVoting.size()*0.0001; x++) {

		
		DrawParabola(imageOut,paramVoting[x].Cx, paramVoting[x].Cy, paramVoting[x].A, paramVoting[x].O);

		parabolaCompo.x0 = paramVoting[x].Cx;		parabolaCompo.y0 = paramVoting[x].Cy;
		parabolaCompo.a = paramVoting[x].A;			parabolaCompo.o = paramVoting[x].O;

		printf("xTrans: %d, yTrans: %d, a:%d, rotat: %d\n", x0, y0, a, o);

		if (x == 3) // voting 값 가장 큰 타원 그림 38
			break;
	}
	paramVoting.clear();
}




// 중심점이 (x0,y0), 장축길이 l, 단축길이 s인 타원 그림
void DrawParabola(CByteImage& imageOut, int x0, int y0, int a, int o) {
	//int x0, int y0, int a, int o;
	
	int nWidth = imageOut.GetWidth();
	int nHeight = imageOut.GetHeight();

	int sliceSize = nWidth*nHeight;
	BYTE* pOut = imageOut.GetPtr();

	
	int x, y, i, j;
	int cnt;
	int num_ang = 360;

	double* tsin = new double[num_ang]; // 0~360도의 sin,cos값 lookup table
	double* tcos = new double[num_ang];

	for (int theta=0; theta<num_ang; theta++) {
		tsin[theta] = (double)sin((double)theta*M_PI / 180);
		tcos[theta] = (double)cos((double)theta*M_PI / 180);
	}

	printf("&d,&d,&d,&d", x0, y0, a, o);

	//x0 = 293;
	//y0 = 143;
	//a = 198;
	//a = 4;
	//o = 0;
	double mt = 0.0;
	mt = (double)(mt - 0.8080);

	for (cnt = 0; cnt<960; cnt ++) {

		x = x0 + (int)(2 * a * mt * tcos[o]) + (int)(a * mt*mt* tsin[o]);			// 포물선 위의 한 점(x,y)
		y = y0 - (int)(2 * a * mt * tsin[o]) + (int)(a * mt*mt* tcos[o]);


		for (i = 0; i<1; i++) {
			for (j = 0; j<1; j++) {
				if ((x + i >= 0 && x + i < nWidth) && (y + j >= 0 && y + j <nHeight)) // 영상 크기 범위 내에서 포물선 그림
					pOut[(x + i) + (y + j) * nWidth] = OBJECT;
			}
		}

		mt += 0.0025;


	}

	delete[] tcos;
	delete[] tsin;
	//	CoronalRegionGrowing(sliceNum-1, x0, y0, w, h); // ellipse 내부 영역을 채움

}
