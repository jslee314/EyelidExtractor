#pragma once

#include "MyImage.h"

#define IN_IMG(x, lo, hi) (x < lo)? lo : x > hi ? hi : x
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#define MAX(a,b)  ((a) < (b) ? (b) : (a))

template <typename T>
CMyImage<T> RGB2Gray(const CMyImage<T>& src)
{
	ASSERT(src.GetChannel() == 3);

	int nWidth  = src.GetWidth();
	int nHeight = src.GetHeight();

	CMyImage<T> dst(nWidth, nHeight);

	for (int r=0 ; r<nHeight ; r++)
	{
		T* pSrc = src.GetPtr(r);
		T* pDst = dst.GetPtr(r);

		int pos = 0;
		for (int c=0 ; c<nWidth ; c++)
		{
//			pDst[c] = (pSrc[pos]+pSrc[pos+1]+pSrc[pos+2])*0.33333;
			pDst[c] = 0.114*pSrc[pos]+0.587*pSrc[pos+1]+0.299*pSrc[pos+2];
			pos += 3;
		}
	}

	return dst;
}

template <typename T>
CMyImage<T> Gray2RGB(const CMyImage<T>& src)
{
	ASSERT(src.GetChannel() == 1);

	int nWidth  = src.GetWidth();
	int nHeight = src.GetHeight();

	CMyImage<T> dst(nWidth, nHeight, 3);

	for (int r=0 ; r<nHeight ; r++)
	{
		T* pSrc = src.GetPtr(r);
		T* pDst = dst.GetPtr(r);

		int pos = 0;
		for (int c=0 ; c<nWidth ; c++)
		{
			pDst[pos  ] = pSrc[c];
			pDst[pos+1] = pSrc[c];
			pDst[pos+2] = pSrc[c];
			pos += 3;
		}
	}

	return dst;
}

template <typename T>
CDoubleImage RGB2HSV(const CMyImage<T>& src)
{
	ASSERT(src.GetChannel() == 3);

	int nWidth  = src.GetWidth();
	int nHeight = src.GetHeight();

	CDoubleImage dst(nWidth, nHeight, 3);

	double vR, vG, vB;
	double vMin, vMax, delta, H;
	for (int r=0 ; r<nHeight ; r++)
	{
		T*		pSrc = src.GetPtr(r);
		double* pDst = dst.GetPtr(r);

		int pos = 0;
		for (int c=0 ; c<nWidth ; c++)
		{
			vB = pSrc[pos  ] / 255.0;
			vG = pSrc[pos+1] / 255.0;
			vR = pSrc[pos+2] / 255.0;

			vMax = MAX(MAX(vR, vG), vB);
			vMin = MIN(MIN(vR, vG), vB);
			delta = vMax - vMin;
			pDst[pos] = vMax;				// V

			if (delta==0)					// Gray
			{
				pDst[pos+1] = 0;
				pDst[pos+2] = 0;
			}
			else 
			{
				pDst[pos+1] = delta / vMax;	// S

				if (vR==vMax)
					H = (vG-vB)/delta;		// 노란색과 자홍색 사이
				else if (vG==vMax)
					H = 2 + (vB-vR)/delta;	// 하늘색과 노란색 사이
				else
					H = 4 + (vR-vG)/delta;	// 자홍색과 하늘색 사이
				
 				H *= 60.0;
				if (H<0)
					H += 360.0;
				pDst[pos+2] = H;			// H
			}

			pos += 3;
		}
	}

	return dst;
}

template <typename T>
CDoubleImage HSV2RGB(const CMyImage<T>& src)
{
	ASSERT(src.GetChannel() == 3);

	int nWidth  = src.GetWidth();
	int nHeight = src.GetHeight();

	CDoubleImage dst(nWidth, nHeight, 3);

	double vS, vH, vV;
	double f, p, t, n;

	for (int r=0 ; r<nHeight ; r++)
	{
		T*		pSrc = src.GetPtr(r);
		double*	pDst = dst.GetPtr(r);

		int pos = 0;
		for (int c=0 ; c<nWidth ; c++)
		{
			vV = pSrc[pos  ];
			vS = pSrc[pos+1];
			vH = pSrc[pos+2];
			if (vS==0)
			{
				pDst[pos  ] = vV*255;
				pDst[pos+1] = vV*255;
				pDst[pos+2] = vV*255;
			}
			else
			{
				while (vH >= 360)
					vH -= 360;
				while (vH < 0)
					vH += 360;
				vH /= 60.0;
				int k = (int)vH;
				f = vH - k;
				t = vV * (1 - vS);
				n = vV * (1 - vS*f);
				p = vV * (1 - vS*(1-f));

				switch (k) // 6개의 구간에 따라
				{
				case 1:
					pDst[pos+2] = n*255;
					pDst[pos+1] = vV*255;
					pDst[pos  ] = t*255;
					break;
				case 2:
					pDst[pos+2] = t*255;
					pDst[pos+1] = vV*255;
					pDst[pos  ] = p*255;
					break;
				case 3:
					pDst[pos+2] = t*255;
					pDst[pos+1] = n*255;
					pDst[pos  ] = vV*255;
					break;
				case 4:
					pDst[pos+2] = p*255;
					pDst[pos+1] = t*255;
					pDst[pos  ] = vV*255;
					break;
				case 5:
					pDst[pos+2] = vV*255;
					pDst[pos+1] = t*255;
					pDst[pos  ] = n*255;
					break;
				default: // case 0
					pDst[pos+2] = vV*255;
					pDst[pos+1] = p*255;
					pDst[pos  ] = t*255;
					break;
				}
			}
			pos += 3;
		}
	}

	return dst;
}


template <typename T1, typename T2, typename TO>
void AddImage(const CMyImage<T1>& src1, const CMyImage<T2>& src2, CMyImage<TO>& dst)
{
	int nWStep  = dst.GetWidth()*dst.GetChannel();
	int nHeight = dst.GetHeight();

	int r, c;

	if (sizeof(TO)==1) // BYTE형 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc1 = src1.GetPtr(r);
			T2* pSrc2 = src2.GetPtr(r);
			TO* pDst  = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = CLIP(pSrc1[c] + pSrc2[c]);
			}
		}
	}
	else // BYTE형 이외의 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc1 = src1.GetPtr(r);
			T2* pSrc2 = src2.GetPtr(r);
			TO* pDst  = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = pSrc1[c] + pSrc2[c];
			}
		}
	}
}

template <typename T1, typename T2>
CMyImage<T1> operator+(const CMyImage<T1>& src1, const CMyImage<T2>& src2)
{
	int nWidth1  = src1.GetWidth();
	int nHeight1 = src1.GetHeight();
	int nWidth2  = src2.GetWidth();
	int nHeight2 = src2.GetHeight();
	int nChan1   = src1.GetChannel();
	int nChan2   = src2.GetChannel();

	ASSERT(nWidth1==nWidth2 && nHeight1==nHeight2 && nChan1==nChan2);

	CMyImage<T1> ret(nWidth1, nHeight1, nChan1);
	AddImage(src1, src2, ret);
	return ret;
}

template <typename T1, typename T2, typename TO>
void AddConst(const CMyImage<T1>& src, const T2 val, CMyImage<TO>& dst)
{
	int nWStep  = dst.GetWidth()*dst.GetChannel();
	int nHeight = dst.GetHeight();

	int r, c;

	if (sizeof(TO)==1) // BYTE형 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc = src.GetPtr(r);
			TO* pDst = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = CLIP(pSrc[c] + val);
			}
		}
	}
	else // BYTE형 이외의 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc = src.GetPtr(r);
			TO* pDst = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = pSrc[c] + val;
			}
		}
	}
}

template <typename T1, typename T2>
CMyImage<T1> operator+(const CMyImage<T1>& src, const T2 val)
{
	int nWidth  = src.GetWidth();
	int nHeight = src.GetHeight();
	int nChan   = src.GetChannel();

	CMyImage<T1> ret(nWidth, nHeight, nChan);
	AddConst(src, val, ret);
	return ret;
}

template <typename T1, typename T2, typename TO>
void SubImage(const CMyImage<T1>& src1, const CMyImage<T2>& src2, CMyImage<TO>& dst)
{
	int nWStep  = dst.GetWidth()*dst.GetChannel();
	int nHeight = dst.GetHeight();

	int r, c;

	if (sizeof(TO)==1) // BYTE형 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc1 = src1.GetPtr(r);
			T2* pSrc2 = src2.GetPtr(r);
			TO* pDst  = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = CLIP(pSrc1[c] - pSrc2[c]);
			}
		}
	}
	else // BYTE형 이외의 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc1 = src1.GetPtr(r);
			T2* pSrc2 = src2.GetPtr(r);
			TO* pDst  = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = pSrc1[c] - pSrc2[c];
			}
		}
	}
}

template <typename T1, typename T2>
CMyImage<T1> operator-(const CMyImage<T1>& src1, const CMyImage<T2>& src2)
{
	int nWidth1  = src1.GetWidth();
	int nHeight1 = src1.GetHeight();
	int nWidth2  = src2.GetWidth();
	int nHeight2 = src2.GetHeight();
	int nChan1   = src1.GetChannel();
	int nChan2   = src2.GetChannel();

	ASSERT(nWidth1==nWidth2 && nHeight1==nHeight2 && nChan1==nChan2);

	CMyImage<T1> ret(nWidth1, nHeight1, nChan1);
	SubImage(src1, src2, ret);
	return ret;
}

template <typename T1, typename T2, typename TO>
void SubConst(const CMyImage<T1>& src, const T2 val, CMyImage<TO>& dst)
{
	int nWStep  = dst.GetWidth()*dst.GetChannel();
	int nHeight = dst.GetHeight();

	int r, c;

	if (sizeof(TO)==1) // BYTE형 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc = src.GetPtr(r);
			TO* pDst = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = CLIP(pSrc[c] - val);
			}
		}
	}
	else // BYTE형 이외의 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc = src.GetPtr(r);
			TO* pDst = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = pSrc[c] - val;
			}
		}
	}
}

template <typename T1, typename T2>
CMyImage<T1> operator-(const CMyImage<T1>& src, const T2 val)
{
	int nWidth  = src.GetWidth();
	int nHeight = src.GetHeight();
	int nChan   = src.GetChannel();

	CMyImage<T1> ret(nWidth, nHeight, nChan);
	SubConst(src, val, ret);
	return ret;
}

template <typename T1, typename T2, typename TO>
void MulImage(const CMyImage<T1>& src1, const CMyImage<T2>& src2, CMyImage<TO>& dst)
{
	int nWStep  = dst.GetWidth()*dst.GetChannel(); 
	int nHeight = dst.GetHeight();

	int r, c;

	if (sizeof(TO)==1) // BYTE형 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc1 = src1.GetPtr(r);
			T2* pSrc2 = src2.GetPtr(r);
			TO* pDst  = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = CLIP(pSrc1[c] * pSrc2[c]);
			}
		}
	}
	else // BYTE형 이외의 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc1 = src1.GetPtr(r);
			T2* pSrc2 = src2.GetPtr(r);
			TO* pDst  = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = pSrc1[c] * pSrc2[c];
			}
		}
	}
}

template <typename T1, typename T2>
CMyImage<T1> operator*(const CMyImage<T1>& src1, const CMyImage<T2>& src2)
{
	int nWidth1  = src1.GetWidth();
	int nHeight1 = src1.GetHeight();
	int nWidth2  = src2.GetWidth();
	int nHeight2 = src2.GetHeight();
	int nChan1   = src1.GetChannel();
	int nChan2   = src2.GetChannel();

	ASSERT(nWidth1==nWidth2 && nHeight1==nHeight2 && nChan1==nChan2);

	CMyImage<T1> ret(nWidth1, nHeight1, nChan1);
	MulImage(src1, src2, ret);
	return ret;
}

template <typename T1, typename T2, typename TO>
void MulConst(const CMyImage<T1>& src, const T2 val, CMyImage<TO>& dst)
{
	int nWStep  = dst.GetWidth()*dst.GetChannel();
	int nHeight = dst.GetHeight();

	int r, c;

	if (sizeof(TO)==1) // BYTE형 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc = src.GetPtr(r);
			TO* pDst = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = CLIP(pSrc[c] * val);
			}
		}
	}
	else // BYTE형 이외의 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc = src.GetPtr(r);
			TO* pDst = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = pSrc[c] * val;
			}
		}
	}
}

template <typename T1, typename T2>
CMyImage<T1> operator*(const CMyImage<T1>& src, const T2 val)
{
	int nWidth  = src.GetWidth();
	int nHeight = src.GetHeight();
	int nChan   = src.GetChannel();

	CMyImage<T1> ret(nWidth, nHeight, nChan);
	MulConst(src, val, ret);
	return ret;
}

template <typename T1, typename T2, typename TO>
void DivImage(const CMyImage<T1>& src1, const CMyImage<T2>& src2, CMyImage<TO>& dst)
{
	int nWStep  = dst.GetWidth()*dst.GetChannel();
	int nHeight = dst.GetHeight();

	int r, c;

	if (sizeof(TO)==1) // BYTE형 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc1 = src1.GetPtr(r);
			T2* pSrc2 = src2.GetPtr(r);
			TO* pDst  = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				if (pSrc2[c]!=0)
					pDst[c] = CLIP(pSrc1[c] / pSrc2[c]);
				else
					pDst[c] = 255;
			}
		}
	}
	else // BYTE형 이외의 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc1 = src1.GetPtr(r);
			T2* pSrc2 = src2.GetPtr(r);
			TO* pDst  = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = pSrc1[c] / pSrc2[c];
			}
		}
	}
}

template <typename T1, typename T2>
CMyImage<T1> operator/(const CMyImage<T1>& src1, const CMyImage<T2>& src2)
{
	int nWidth1  = src1.GetWidth();
	int nHeight1 = src1.GetHeight();
	int nWidth2  = src2.GetWidth();
	int nHeight2 = src2.GetHeight();
	int nChan1   = src1.GetChannel();
	int nChan2   = src2.GetChannel();

	ASSERT(nWidth1==nWidth2 && nHeight1==nHeight2 && nChan1==nChan2);

	CMyImage<T1> ret(nWidth1, nHeight1, nChan1);
	DivImage(src1, src2, ret);
	return ret;
}

template <typename T1, typename T2, typename TO>
void DivConst(const CMyImage<T1>& src, const T2 val, CMyImage<TO>& dst)
{
	int nWStep  = dst.GetWidth()*dst.GetChannel();
	int nHeight = dst.GetHeight();

	int r, c;

	if (sizeof(TO)==1) // BYTE형 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc = src.GetPtr(r);
			TO* pDst = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = CLIP(pSrc[c] / val);
			}
		}
	}
	else // BYTE형 이외의 영상
	{
		for (r=0 ; r<nHeight ; r++)
		{
			T1* pSrc = src.GetPtr(r);
			TO* pDst = dst.GetPtr(r);

			for (c=0 ; c<nWStep ; c++)
			{
				pDst[c] = pSrc[c] / val;
			}
		}
	}
}

template <typename T1, typename T2>
CMyImage<T1> operator/(const CMyImage<T1>& src, const T2 val)
{
	int nWidth  = src.GetWidth();
	int nHeight = src.GetHeight();
	int nChan   = src.GetChannel();

	CMyImage<T1> ret(nWidth, nHeight, nChan);
	DivConst(src, val, ret);
	return ret;
}

// 논리 연산자 선언
void ANDImage(const CByteImage& src1, const CByteImage& src2, CByteImage& dst);
CByteImage operator&(const CByteImage& src1, const CByteImage& src2);
void ANDConst(const CByteImage& src, const BYTE val, CByteImage& dst);
CByteImage operator&(const CByteImage& src, const BYTE val);
void ORImage(const CByteImage& src1, const CByteImage& src2, CByteImage& dst);
CByteImage operator|(const CByteImage& src1, const CByteImage& src2);
void ORConst(const CByteImage& src, const BYTE val, CByteImage& dst);
CByteImage operator|(const CByteImage& src, const BYTE val);
void NOTImage(const CByteImage& src, CByteImage& dst);
CByteImage operator~(const CByteImage& src);
void XORImage(const CByteImage& src1, const CByteImage& src2, CByteImage& dst);
CByteImage operator^(const CByteImage& src1, const CByteImage& src2);
void XORConst(const CByteImage& src, const BYTE val, CByteImage& dst);
CByteImage operator^(const CByteImage& src, const BYTE val);
