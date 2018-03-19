#include "stdafx.h"
#include "Segmentation.h"

void Binarization(const CByteImage& imageIn, CByteImage& imageOut, int nThreshold, int objGL)
{


	ASSERT(imageIn.GetChannel() == 1);

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	for (int r = 0; r<nHeight; r++)
	{
		BYTE* pIn = imageIn.GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		for (int c = 0; c<nWidth; c++)
		{
			if (pIn[c] > nThreshold)
				pOut[c] = objGL;
			else
				pOut[c] = (OBJECT-objGL);
		}
	}
}

int BinarizationAuto(const CByteImage& imageIn, CByteImage& imageOut, int nThreshold)
{
	ASSERT(imageIn.GetChannel() == 1);

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	unsigned int nSumHi, nSumLo; // 두 영역 밝기의 합
	unsigned int nNumHi, nNumLo; // 두 영역의 픽셀 수
	bool bChanged = true;

	while (bChanged)
	{
		bChanged = false;
		nSumHi = nSumLo = 0; // 합 초기화
		nNumHi = nNumLo = 0; // 개수 초기화

		for (int r = 0; r<nHeight; r++)
		{
			BYTE *pIn = imageIn.GetPtr(r);
			BYTE *pOut = imageOut.GetPtr(r);

			for (int c = 0; c<nWidth; c++)
			{
				if (pIn[c] > nThreshold)
				{
					nSumHi += pIn[c];	 // 밝기 합 더하기
					nNumHi++;			 // 픽셀 수 증가	
					if (!pOut[c])		 // 이전에 문턱값 이하
						bChanged = true; // 분할에 변화 있음
					pOut[c] = OBJECT;
				}
				else
				{
					nSumLo += pIn[c];    // 밝기 합 더하기
					nNumLo++;		     // 픽셀 수 증가
					if (pOut[c])	     // 이전에 문턱값 초과
						bChanged = true; // 분할에 변화 있음
					pOut[c] = BACKGROUND;
				}
			}
		}

		if (!nNumHi)
			nThreshold = nSumLo / (double)nNumLo;
		else if (!nNumLo)
			nThreshold = nSumHi / (double)nNumHi;
		else
			nThreshold = (nSumHi / (double)nNumHi + nSumLo / (double)nNumLo) / 2;
	} // while (bChanged)

	return nThreshold; // 최종 문턱값
}

void BinarizationBG(const CByteImage& imageIn, const CByteImage& imageBG,
	CByteImage& imageOut, int nThreshold)
{
	ASSERT(imageIn.GetChannel() == 1); // 회색조 영상 확인

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	for (int r = 0; r<nHeight; r++)
	{
		BYTE* pIn = imageIn.GetPtr(r);
		BYTE* pBG = imageBG.GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		for (int c = 0; c<nWidth; c++){
			if (abs(pIn[c] - pBG[c]) > nThreshold)
				pOut[c] = 255;
			else
				pOut[c] = 0;
		}
	}
}

void BinarizationBGCol(const CByteImage& imageIn, const CByteImage& imageBG,
	CByteImage& imageOut, int nThreshold)
{
	ASSERT(imageIn.GetChannel() == 3); // 컬러 영상 확인

	nThreshold *= nThreshold; // 문턱값을 제곱

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	int dB, dG, dR, dd;
	for (int r = 0; r<nHeight; r++)
	{
		BYTE* pIn = imageIn.GetPtr(r);
		BYTE* pBG = imageBG.GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		int pos = 0;
		for (int c = 0; c<nWidth; c++)
		{
			dB = pIn[pos] - pBG[pos]; pos++;
			dG = pIn[pos] - pBG[pos]; pos++;
			dR = pIn[pos] - pBG[pos]; pos++;
			dd = dB*dB + dG*dG + dR*dR;
			if (dd > nThreshold) // 제곱끼리 비교
				pOut[c] = 255;
			else
				pOut[c] = 0;
		}
	}
}

void KMeansSegmentation(const CByteImage& imageIn, CByteImage& imageOut, int nCluster, BYTE meanR[], BYTE meanG[], BYTE meanB[])
{
	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	CByteImage imageK = CByteImage(nWidth, nHeight);

	bool bChanged = true;

	int cR, cG, cB, dR, dG, dB, dd, minD;
	int newK;

	unsigned int nSumR[MAX_CLUSTER]; // 각 영역 R 채널 합
	unsigned int nSumG[MAX_CLUSTER]; // 각 영역 G 채널 합
	unsigned int nSumB[MAX_CLUSTER]; // 각 영역 B 채널 합
	unsigned int nNumP[MAX_CLUSTER]; // 각 영역 픽셀 수

	while (bChanged){
		bChanged = false;

		memset(nSumB, 0, sizeof(int)*nCluster); // B채널 합 초기화
		memset(nSumG, 0, sizeof(int)*nCluster); // G채널 합 초기화
		memset(nSumR, 0, sizeof(int)*nCluster); // R채널 합 초기화
		memset(nNumP, 0, sizeof(int)*nCluster); // 픽셀 수 초기화

		for (int r = 0; r<nHeight; r++)
		{
			BYTE *pIn = imageIn.GetPtr(r);
			BYTE *pK = imageK.GetPtr(r);

			int pos = 0;
			for (int c = 0; c<nWidth; c++)
			{
				// 현재 위치의 픽셀 값
				cB = pIn[pos++];
				cG = pIn[pos++];
				cR = pIn[pos++];

				// 가장 가까운 분류 탐색 (m_imageK 갱신)
				minD = INT_MAX;
				newK = 0;
				for (int k = 0; k<nCluster; k++)
				{
					dB = cB - meanB[k];
					dG = cG - meanG[k];
					dR = cR - meanR[k];
					dd = dB*dB + dG*dG + dR*dR;

					if (dd < minD)
					{
						minD = dd;
						newK = k;
					}
				}

				if (newK != pK[c])
					bChanged = true;

				pK[c] = newK; // 가장 가까운 값으로 변경

							  // 평균을 구하기 위한 합 계산
				nSumB[newK] += cB;
				nSumG[newK] += cG;
				nSumR[newK] += cR;
				nNumP[newK]++;
			}
		}

		// 새로운 평균 계산 (m_meanC 갱신)
		for (int k = 0; k<nCluster; k++)
		{
			if (nNumP[k])
			{
				meanB[k] = nSumB[k] / (double)nNumP[k];
				meanG[k] = nSumG[k] / (double)nNumP[k];
				meanR[k] = nSumR[k] / (double)nNumP[k];
			}
			else
			{
				meanB[k] = 0;
				meanG[k] = 0;
				meanR[k] = 0;
			}
		}
	} // while (bChanged)

	  // 결과 영상 생성
	int curK;
	for (int r = 0; r<nHeight; r++)
	{
		BYTE* pOut = imageOut.GetPtr(r);
		BYTE* pK = imageK.GetPtr(r);

		int pos = 0;
		for (int c = 0; c<nWidth; c++)
		{
			curK = pK[c];
			pOut[pos++] = meanB[curK];
			pOut[pos++] = meanG[curK];
			pOut[pos++] = meanR[curK];
		}
	}
}

