#include "stdafx.h"
#include "MorphologyFilter.h"

void Erode(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut)
{
	// 입력 영상 및 마스크 영상의 크기 정보
	int nWidth  = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nMaskW = mask.GetWidth();
	int nMaskH = mask.GetHeight();
	int hW = nMaskW/2;
	int hH = nMaskH/2;

	for (int r=hH ; r<nHeight-hH ; r++) // 입력 영상의 세로 방향
	{
		BYTE* pIn  = imageIn .GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		for (int c=hW ; c<nWidth-hW ; c++) // 입력 영상의 가로 방향
		{
			if (!pIn[c]) // 값이 0인 입력 픽셀은 처리하지 않음
				continue;

			pOut[c] = 255;
			bool bEroded = false;
			for (int y=0 ; y<nMaskH && !bEroded ; y++) // 마스크의 세로 방향
			{
				BYTE* pMask = mask.GetPtr(y);
				BYTE* pInM = imageIn.GetPtr(r-hH+y, c-hW); // 입력 영상에서 마스크 내의 각 행

				for (int x=0 ; x<nMaskW && !bEroded ; x++) // 마스크의 가로 방향
				{
					if (pMask[x]) // 마스크 값이 0이 아니면 입력 픽셀을 검사
					{
						if (!pInM[x]) // 한 픽셀이라도 0 값을 가지면
						{
							pOut[c] = 0;	// 결과 값은 0으로
							bEroded = true;	// 더 이상의 검사를 중단
						}
					}
				}
			}
		}
	}
}

void Dilate(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut)
{
	// 입력 영상 및 마스크 영상의 크기 정보
	int nWidth  = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nMaskW = mask.GetWidth();
	int nMaskH = mask.GetHeight();
	int hW = nMaskW/2;
	int hH = nMaskH/2;

	for (int r=hH ; r<nHeight-hH ; r++) // 입력 영상의 세로 방향
	{
		BYTE* pIn  = imageIn .GetPtr(r);

		for (int c=hW ; c<nWidth-hW ; c++) // 입력 영상의 가로 방향
		{
			if (!pIn[c]) // 값이 0인 입력 픽셀은 처리하지 않음
				continue;

			for (int y=0 ; y<nMaskH ; y++) // 마스크의 세로 방향
			{
				BYTE* pMask = mask.GetPtr(y);
				BYTE* pOut = imageOut.GetPtr(r-hH+y, c-hW); // 입력 영상에서 마스크 내의 각 행

				for (int x=0 ; x<nMaskW ; x++) // 마스크의 가로 방향
				{
					if (pMask[x]) // 마스크 값이 유효하면
					{
						pOut[x] = OBJECT; // 결과 값을 255로
					}
				}
			}
		}
	}
}


void Dilate_horiz(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut)
{
	// 입력 영상 및 마스크 영상의 크기 정보
	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nMaskW = mask.GetWidth();
	int nMaskH = mask.GetHeight();
	int hW = nMaskW / 2;
	int hH = nMaskH / 2;

	for (int r = hH; r<nHeight - hH; r++) // 입력 영상의 세로 방향
	{
		BYTE* pIn = imageIn.GetPtr(r);

		for (int c = hW; c<nWidth - hW; c++) // 입력 영상의 가로 방향
		{
			if (!pIn[c]) // 값이 0인 입력 픽셀은 처리하지 않음
				continue;

			//for (int y = 0; y<nMaskH; y++) // 마스크의 세로 방향
		//	{
			int y = hH;
				BYTE* pMask = mask.GetPtr(y);
				BYTE* pOut = imageOut.GetPtr(r - hH + y, c - hW); // 입력 영상에서 마스크 내의 각 행

				for (int x = 0; x<nMaskW; x++) // 마스크의 가로 방향
				{
					if (pMask[x]) // 마스크 값이 유효하면
					{
						pOut[x] = OBJECT; // 결과 값을 255로
					}
				}
		//	}
		}
	}


}

void ErodeG(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut)
{
	// 입력 영상 및 마스크 영상의 크기 정보
	int nWidth  = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nMaskW = mask.GetWidth();
	int nMaskH = mask.GetHeight();
	int hW = nMaskW/2;
	int hH = nMaskH/2;

	for (int r=hH ; r<nHeight-hH ; r++) // 입력 영상의 세로 방향
	{
		BYTE* pIn  = imageIn .GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		for (int c=hW ; c<nWidth-hW ; c++) // 입력 영상의 가로 방향
		{
			int minVal = 255;
			for (int y=0 ; y<nMaskH ; y++) // 마스크의 세로 방향
			{
				BYTE* pMask = mask.GetPtr(y);
				BYTE* pInM = imageIn.GetPtr(r-hH+y, c-hW); // 입력 영상에서 마스크 내의 각 행

				for (int x=0 ; x<nMaskW ; x++) // 마스크의 가로 방향
				{
					int diff = pInM[x] - pMask[x];
					if (diff < minVal) // 최소값 탐색
					{
						minVal = diff;
					}
				}
			}

			pOut[c] = CLIP(minVal); // 결과 값이 [0, 255] 구간에 오도록 조절
		}
	}
}

void DilateG(const CByteImage& imageIn, const CByteImage& mask, CByteImage& imageOut)
{
	// 입력 영상 및 마스크 영상의 크기 정보
	int nWidth  = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nMaskW = mask.GetWidth();
	int nMaskH = mask.GetHeight();
	int hW = nMaskW/2;
	int hH = nMaskH/2;

	for (int r=hH ; r<nHeight-hH ; r++) // 입력 영상의 세로 방향
	{
		BYTE* pIn  = imageIn .GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		for (int c=hW ; c<nWidth-hW ; c++) // 입력 영상의 가로 방향
		{
			int maxVal = 0;
			for (int y=0 ; y<nMaskH ; y++) // 마스크의 세로 방향
			{
				BYTE* pMask = mask.GetPtr(y);
				BYTE* pInM = imageIn.GetPtr(r-hH+y, c-hW); // 입력 영상에서 마스크 내의 각 행

				for (int x=0 ; x<nMaskW ; x++) // 마스크의 가로 방향
				{
					int sum = pInM[x] + pMask[x];
					if (sum > maxVal) // 최대값 탐색 
					{
						maxVal = sum;
					}
				}
			}

			pOut[c] = CLIP(maxVal); // 결과 값이 [0, 255] 구간에 오도록 조절
		}
	}
}