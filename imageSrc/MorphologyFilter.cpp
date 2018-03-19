#include "stdafx.h"
#include "MorphologyFilter.h"

void Erode(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut)
{
	// �Է� ���� �� ����ũ ������ ũ�� ����
	int nWidth  = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nMaskW = mask.GetWidth();
	int nMaskH = mask.GetHeight();
	int hW = nMaskW/2;
	int hH = nMaskH/2;

	for (int r=hH ; r<nHeight-hH ; r++) // �Է� ������ ���� ����
	{
		BYTE* pIn  = imageIn .GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		for (int c=hW ; c<nWidth-hW ; c++) // �Է� ������ ���� ����
		{
			if (!pIn[c]) // ���� 0�� �Է� �ȼ��� ó������ ����
				continue;

			pOut[c] = 255;
			bool bEroded = false;
			for (int y=0 ; y<nMaskH && !bEroded ; y++) // ����ũ�� ���� ����
			{
				BYTE* pMask = mask.GetPtr(y);
				BYTE* pInM = imageIn.GetPtr(r-hH+y, c-hW); // �Է� ���󿡼� ����ũ ���� �� ��

				for (int x=0 ; x<nMaskW && !bEroded ; x++) // ����ũ�� ���� ����
				{
					if (pMask[x]) // ����ũ ���� 0�� �ƴϸ� �Է� �ȼ��� �˻�
					{
						if (!pInM[x]) // �� �ȼ��̶� 0 ���� ������
						{
							pOut[c] = 0;	// ��� ���� 0����
							bEroded = true;	// �� �̻��� �˻縦 �ߴ�
						}
					}
				}
			}
		}
	}
}

void Dilate(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut)
{
	// �Է� ���� �� ����ũ ������ ũ�� ����
	int nWidth  = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nMaskW = mask.GetWidth();
	int nMaskH = mask.GetHeight();
	int hW = nMaskW/2;
	int hH = nMaskH/2;

	for (int r=hH ; r<nHeight-hH ; r++) // �Է� ������ ���� ����
	{
		BYTE* pIn  = imageIn .GetPtr(r);

		for (int c=hW ; c<nWidth-hW ; c++) // �Է� ������ ���� ����
		{
			if (!pIn[c]) // ���� 0�� �Է� �ȼ��� ó������ ����
				continue;

			for (int y=0 ; y<nMaskH ; y++) // ����ũ�� ���� ����
			{
				BYTE* pMask = mask.GetPtr(y);
				BYTE* pOut = imageOut.GetPtr(r-hH+y, c-hW); // �Է� ���󿡼� ����ũ ���� �� ��

				for (int x=0 ; x<nMaskW ; x++) // ����ũ�� ���� ����
				{
					if (pMask[x]) // ����ũ ���� ��ȿ�ϸ�
					{
						pOut[x] = OBJECT; // ��� ���� 255��
					}
				}
			}
		}
	}
}


void Dilate_horiz(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut)
{
	// �Է� ���� �� ����ũ ������ ũ�� ����
	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nMaskW = mask.GetWidth();
	int nMaskH = mask.GetHeight();
	int hW = nMaskW / 2;
	int hH = nMaskH / 2;

	for (int r = hH; r<nHeight - hH; r++) // �Է� ������ ���� ����
	{
		BYTE* pIn = imageIn.GetPtr(r);

		for (int c = hW; c<nWidth - hW; c++) // �Է� ������ ���� ����
		{
			if (!pIn[c]) // ���� 0�� �Է� �ȼ��� ó������ ����
				continue;

			//for (int y = 0; y<nMaskH; y++) // ����ũ�� ���� ����
		//	{
			int y = hH;
				BYTE* pMask = mask.GetPtr(y);
				BYTE* pOut = imageOut.GetPtr(r - hH + y, c - hW); // �Է� ���󿡼� ����ũ ���� �� ��

				for (int x = 0; x<nMaskW; x++) // ����ũ�� ���� ����
				{
					if (pMask[x]) // ����ũ ���� ��ȿ�ϸ�
					{
						pOut[x] = OBJECT; // ��� ���� 255��
					}
				}
		//	}
		}
	}


}

void ErodeG(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut)
{
	// �Է� ���� �� ����ũ ������ ũ�� ����
	int nWidth  = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nMaskW = mask.GetWidth();
	int nMaskH = mask.GetHeight();
	int hW = nMaskW/2;
	int hH = nMaskH/2;

	for (int r=hH ; r<nHeight-hH ; r++) // �Է� ������ ���� ����
	{
		BYTE* pIn  = imageIn .GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		for (int c=hW ; c<nWidth-hW ; c++) // �Է� ������ ���� ����
		{
			int minVal = 255;
			for (int y=0 ; y<nMaskH ; y++) // ����ũ�� ���� ����
			{
				BYTE* pMask = mask.GetPtr(y);
				BYTE* pInM = imageIn.GetPtr(r-hH+y, c-hW); // �Է� ���󿡼� ����ũ ���� �� ��

				for (int x=0 ; x<nMaskW ; x++) // ����ũ�� ���� ����
				{
					int diff = pInM[x] - pMask[x];
					if (diff < minVal) // �ּҰ� Ž��
					{
						minVal = diff;
					}
				}
			}

			pOut[c] = CLIP(minVal); // ��� ���� [0, 255] ������ ������ ����
		}
	}
}

void DilateG(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut)
{
	// �Է� ���� �� ����ũ ������ ũ�� ����
	int nWidth  = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nMaskW = mask.GetWidth();
	int nMaskH = mask.GetHeight();
	int hW = nMaskW/2;
	int hH = nMaskH/2;

	for (int r=hH ; r<nHeight-hH ; r++) // �Է� ������ ���� ����
	{
		BYTE* pIn  = imageIn .GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		for (int c=hW ; c<nWidth-hW ; c++) // �Է� ������ ���� ����
		{
			int maxVal = 0;
			for (int y=0 ; y<nMaskH ; y++) // ����ũ�� ���� ����
			{
				BYTE* pMask = mask.GetPtr(y);
				BYTE* pInM = imageIn.GetPtr(r-hH+y, c-hW); // �Է� ���󿡼� ����ũ ���� �� ��

				for (int x=0 ; x<nMaskW ; x++) // ����ũ�� ���� ����
				{
					int sum = pInM[x] + pMask[x];
					if (sum > maxVal) // �ִ밪 Ž�� 
					{
						maxVal = sum;
					}
				}
			}

			pOut[c] = CLIP(maxVal); // ��� ���� [0, 255] ������ ������ ����
		}
	}
}