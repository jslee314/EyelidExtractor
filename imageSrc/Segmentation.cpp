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

	unsigned int nSumHi, nSumLo; // �� ���� ����� ��
	unsigned int nNumHi, nNumLo; // �� ������ �ȼ� ��
	bool bChanged = true;

	while (bChanged)
	{
		bChanged = false;
		nSumHi = nSumLo = 0; // �� �ʱ�ȭ
		nNumHi = nNumLo = 0; // ���� �ʱ�ȭ

		for (int r = 0; r<nHeight; r++)
		{
			BYTE *pIn = imageIn.GetPtr(r);
			BYTE *pOut = imageOut.GetPtr(r);

			for (int c = 0; c<nWidth; c++)
			{
				if (pIn[c] > nThreshold)
				{
					nSumHi += pIn[c];	 // ��� �� ���ϱ�
					nNumHi++;			 // �ȼ� �� ����	
					if (!pOut[c])		 // ������ ���ΰ� ����
						bChanged = true; // ���ҿ� ��ȭ ����
					pOut[c] = OBJECT;
				}
				else
				{
					nSumLo += pIn[c];    // ��� �� ���ϱ�
					nNumLo++;		     // �ȼ� �� ����
					if (pOut[c])	     // ������ ���ΰ� �ʰ�
						bChanged = true; // ���ҿ� ��ȭ ����
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

	return nThreshold; // ���� ���ΰ�
}

void BinarizationBG(const CByteImage& imageIn, const CByteImage& imageBG,
	CByteImage& imageOut, int nThreshold)
{
	ASSERT(imageIn.GetChannel() == 1); // ȸ���� ���� Ȯ��

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
	ASSERT(imageIn.GetChannel() == 3); // �÷� ���� Ȯ��

	nThreshold *= nThreshold; // ���ΰ��� ����

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
			if (dd > nThreshold) // �������� ��
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

	unsigned int nSumR[MAX_CLUSTER]; // �� ���� R ä�� ��
	unsigned int nSumG[MAX_CLUSTER]; // �� ���� G ä�� ��
	unsigned int nSumB[MAX_CLUSTER]; // �� ���� B ä�� ��
	unsigned int nNumP[MAX_CLUSTER]; // �� ���� �ȼ� ��

	while (bChanged){
		bChanged = false;

		memset(nSumB, 0, sizeof(int)*nCluster); // Bä�� �� �ʱ�ȭ
		memset(nSumG, 0, sizeof(int)*nCluster); // Gä�� �� �ʱ�ȭ
		memset(nSumR, 0, sizeof(int)*nCluster); // Rä�� �� �ʱ�ȭ
		memset(nNumP, 0, sizeof(int)*nCluster); // �ȼ� �� �ʱ�ȭ

		for (int r = 0; r<nHeight; r++)
		{
			BYTE *pIn = imageIn.GetPtr(r);
			BYTE *pK = imageK.GetPtr(r);

			int pos = 0;
			for (int c = 0; c<nWidth; c++)
			{
				// ���� ��ġ�� �ȼ� ��
				cB = pIn[pos++];
				cG = pIn[pos++];
				cR = pIn[pos++];

				// ���� ����� �з� Ž�� (m_imageK ����)
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

				pK[c] = newK; // ���� ����� ������ ����

							  // ����� ���ϱ� ���� �� ���
				nSumB[newK] += cB;
				nSumG[newK] += cG;
				nSumR[newK] += cR;
				nNumP[newK]++;
			}
		}

		// ���ο� ��� ��� (m_meanC ����)
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

	  // ��� ���� ����
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

