#pragma once

#include <windows.h>

#define CLIP(x) (x < 0)? 0 : x > 255 ? 255 : x
#define IN_IMG(x, lo, hi) (x < lo)? lo : x > hi ? hi : x

//#define MIN(x, y) (((x) < (y)) ? (x) : (y))
//#define MAX(x, y) (((x) > (y)) ? (x) : (y))

//jslee

#define OBJECT		255		// ��� object(ȫä, ����, ����Ǯ)
#define BACKGROUND 	0		// ����	background
#define NULLPIX		500		//

//--------------
#define TempLabel		-1
#define MASKNONE		0
#define MASKSEGMENTED	1
#define MASKFENCE		2
#define MASKCONTOUR		4
#define MASKERROR      128
#define LEVINITCONTOUR	5
#define MASKBONE        10
#define MASKVESSELRESTORATION  20
#define LIVERBOUNDARY	7

#define BITMAPNONE  0
#define BITMAPIMAGE 1
#define BITMAPERROR 128

typedef struct Eye_factor{
	int centerX;
	int centerY;

	// radious
	int pupilR;
	int irisR;


}Eye_factor;

typedef struct Roi{
	int stX;
	int stY;
	int endX;
	int endY;

}Roi;

typedef struct parabolaComponent
{
	int x0, y0, a, o;	// �߽���(x0,y0),  ���� o
}parabolaComponent;


template <typename T>
class CMyImage
{
public:
	CMyImage(void)
	: m_nChannels(0)
	, m_nHeight(0)
	, m_nWidth(0)
	, m_nWStep(0)
	, m_pImageData(NULL)
	{
	}

	CMyImage(int nWidth, int nHeight, int nChannels = 1)
	: m_nChannels(nChannels)
	, m_nHeight(nHeight)
	, m_nWidth(nWidth)
	, m_nWStep(((nWidth*nChannels*sizeof(T)+3)&~3)/sizeof(T))
	{
		m_pImageData = new T[m_nHeight*m_nWStep];
	}

	CMyImage(const CMyImage& myImage)
	{
		m_nChannels = myImage.m_nChannels;
		m_nHeight	= myImage.m_nHeight;
		m_nWidth	= myImage.m_nWidth;
		m_nWStep	= myImage.m_nWStep;
		m_pImageData = new T[m_nHeight*m_nWStep];
		m_strPathName = myImage.m_strPathName;
		memcpy(m_pImageData, myImage.m_pImageData, m_nHeight*m_nWStep*sizeof(T));
	}

	CMyImage& operator=(const CMyImage& myImage) 
	{
		if (this == &myImage)
			return *this;

		m_nChannels = myImage.m_nChannels;
		m_nHeight	= myImage.m_nHeight;
		m_nWidth	= myImage.m_nWidth;
		m_nWStep	= myImage.m_nWStep;
		m_strPathName = myImage.m_strPathName;

		if (m_pImageData) 
			delete [] m_pImageData;

		if (myImage.m_pImageData != NULL)
		{
			m_pImageData = new T[m_nHeight*m_nWStep];
			memcpy(m_pImageData, myImage.m_pImageData, m_nHeight*m_nWStep*sizeof(T));
		}
		return *this;
	}

	template <typename From>
	CMyImage(const CMyImage<From>& myImage)
	{
		m_nChannels = myImage.GetChannel();
		m_nHeight	= myImage.GetHeight();
		m_nWidth	= myImage.GetWidth();
		m_nWStep	= ((m_nWidth*m_nChannels*sizeof(T)+3)&~3)/sizeof(T);
		m_pImageData = new T[m_nHeight*m_nWStep];


		int nWStep   = myImage.GetWStep();

		if (sizeof(T)==1)
		{
			for (int r=0 ; r<m_nHeight ; r++)
			{
				T* pDst    = GetPtr(r);
				From* pSrc = myImage.GetPtr(r);
				for (int c=0 ; c<nWStep ; c++)
				{
					pDst[c] = (T)CLIP(pSrc[c]);
				}
			}
		}
		else
		{
			for (int r=0 ; r<m_nHeight ; r++)
			{
				T* pDst    = GetPtr(r);
				From* pSrc = myImage.GetPtr(r);
				for (int c=0 ; c<nWStep ; c++)
				{
					pDst[c] = (T)pSrc[c];
				}
			}
		}
	}

	~CMyImage(void)
	{
		if (m_pImageData) delete [] m_pImageData;
	}

	bool LoadImage(const char* filename)
	{
		ASSERT(sizeof(T)==1); // BYTE���� ��츸 ����

		if (strcmp(".BMP", &filename[strlen(filename)-4]))
		{
			FILE* pFile = NULL;
			fopen_s(&pFile, filename, "rb"); // ���̳ʸ� �б� ���
			if (!pFile)
				return false;

			BITMAPFILEHEADER fileHeader;

			if (!fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pFile))
			{
				fclose(pFile);
				return false;
			}

			if (fileHeader.bfType != 0x4D42) // 'BM' ���� �˻�
			{
				fclose(pFile);
				return false;
			}

			BITMAPINFOHEADER infoHeader;
			if (!fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pFile))
			{
				fclose(pFile);
				return false;
			}

			if (infoHeader.biBitCount != 8 && infoHeader.biBitCount != 24)
			{
				fclose(pFile);
				return false;
			}

			if (m_nWidth != infoHeader.biWidth && m_nHeight != infoHeader.biHeight 
				&& m_nChannels != infoHeader.biBitCount/8)
			{
				if (m_pImageData)
					delete [] m_pImageData;

				m_nChannels = infoHeader.biBitCount/8;
				m_nHeight	= infoHeader.biHeight;
				m_nWidth	= infoHeader.biWidth;
				m_nWStep	= (m_nWidth*m_nChannels*sizeof(T)+3)&~3;

				m_pImageData = new T[m_nHeight*m_nWStep];
				
			}

			fseek(pFile, fileHeader.bfOffBits, SEEK_SET);

			int r;
			for (r=m_nHeight-1 ; r>=0 ; r--)
			{
				if(!fread(&m_pImageData[r*m_nWStep], sizeof(BYTE), m_nWStep, pFile))
				{
					fclose(pFile);
					return false;
				}
			}

			fclose(pFile);

			return true;
		}
		else
		{
			return false;
		}
	}

	bool SaveImage(const char* filename)
	{
		ASSERT(sizeof(T)==1); // BYTE���� ��츸 ����

		if (strcmp(".BMP", &filename[strlen(filename)-4]))
		{
			FILE* pFile = NULL;
			fopen_s(&pFile, filename, "wb");
			if (!pFile)
				return false;

			BITMAPFILEHEADER fileHeader;
			fileHeader.bfType = 0x4D42; // 'BM'
			fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + m_nWStep*m_nHeight + (m_nChannels == 1)*1024;
			fileHeader.bfReserved1 = 0;
			fileHeader.bfReserved2 = 0;
			fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (m_nChannels == 1)*256*sizeof(RGBQUAD);

			fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);

			BITMAPINFOHEADER infoHeader;
			infoHeader.biSize = sizeof(BITMAPINFOHEADER);
			infoHeader.biWidth = m_nWidth;
			infoHeader.biHeight = m_nHeight;
			infoHeader.biPlanes = 1;
			infoHeader.biBitCount = m_nChannels*8;
			infoHeader.biCompression = BI_RGB;
			infoHeader.biSizeImage = m_nWStep*m_nHeight;
			infoHeader.biClrImportant = 0;
			infoHeader.biClrUsed = 0;
			infoHeader.biXPelsPerMeter = 0;
			infoHeader.biYPelsPerMeter = 0;

			fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

			if (m_nChannels == 1)
			{
				for (int l=0 ; l<256 ; l++)
				{
					RGBQUAD GrayPalette = {l, l, l, 0};
					fwrite(&GrayPalette, sizeof(RGBQUAD), 1, pFile);
				}
			}

			int r;
			for (r=m_nHeight-1 ; r>=0 ; r--)
			{
				fwrite(&m_pImageData[r*m_nWStep], sizeof(BYTE), m_nWStep, pFile);
			}

			fclose(pFile);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool IsEmpty() const
	{
		return m_pImageData? false : true;
	}

	void SetConstValue(T val)
	{
		if (val == 0)   // ���� (1)
		{
			memset(m_pImageData, 0, m_nWStep*m_nHeight*sizeof(T));
			return;
		}

		if (sizeof(T)==1)  // ���� (2)
		{
			memset(m_pImageData, val, m_nWStep*m_nHeight);
		}
		else  // ������ ����
		{
			T* pData = m_pImageData;
			for (int r=0 ; r<m_nHeight ; r++)
			{
				for (int c=0 ; c<m_nWidth ; c++)
				{
					pData[c] = val;
				}
				pData += m_nWStep;
			}
		}
	}

	inline T& GetAt(int x, int y, int c=0) const
	{
		ASSERT(x >= 0 && x < m_nWidth && y >= 0 && y < m_nHeight);
		return m_pImageData[m_nWStep*y + m_nChannels*x + c];
	}

	//------------------------------------------
	void PutChannelImg(const CMyImage<T>& src, int nChannel)
	{
		int nWidth  = src.GetWidth();
		int nHeight = src.GetHeight();

		for (int r=0 ; r<nHeight ; r++)
		{
			T* pSrc = src.GetPtr(r);
			T* pDst = GetPtr(r);

			int idx = nChannel;
			for (int c=0 ; c<nWidth ; c++)
			{
				pDst[idx] = pSrc[c];
				idx += m_nChannels;
			}
		}
	}

	CMyImage<T> GetChannelImg(int nChannel) const
	{
		CMyImage<T> dst(m_nWidth, m_nHeight);

		for (int r=0 ; r<m_nHeight ; r++)
		{
			T* pSrc = GetPtr(r);
			T* pDst = dst.GetPtr(r);

			int idx = nChannel;
			for (int c=0 ; c<m_nWidth ; c++)
			{
				pDst[c] = pSrc[idx];
				idx += m_nChannels;
			}
		}

		return dst;
	}

	CMyImage<T> Transpose() const
	{
		// ��ȯ�� ��� ���� ����
		CMyImage<T> dst(m_nHeight, m_nWidth, m_nChannels);

		T* pSrc = m_pImageData;
		T* pDst = dst.GetPtr();

		int nWStepS = GetWStep();
		int nWStepD = dst.GetWStep();
		int nBack = (nWStepS*m_nHeight - m_nChannels);

		for (int r=0 ; r<m_nHeight ; r++)
		{
			int idx = 0;
			for (int c=0 ; c<m_nWidth ; c++)
			{
				// �� ä�� ���� ������� ����
				for (int h=0 ; h<m_nChannels ; h++)
				{
					pDst[h] = pSrc[idx+h];
				}
				idx += m_nChannels; // ���� ������ ������ �ȼ��� �̵�
				pDst += nWStepD; // ��� ������ �Ʒ��� �ȼ��� �̵�
			}
			pSrc += nWStepS; // ���� ������ ���� ������ �̵�
			pDst -= nBack; // ��� ������ ���� ���� �̵�
		}

		return dst;
	}

	BYTE NearestNeighbor(double x, double y)
	{
		int px = IN_IMG((int)(x+0.5), 0, m_nWidth);
		int py = IN_IMG((int)(y+0.5), 0, m_nHeight);
		return m_pImageData[py*m_nWStep + px];
	}

	BYTE BiLinearIntp(double x, double y)
	{
		double wLT, wRT, wLB, wRB;

		int xL = IN_IMG((int)x  , 0, m_nWidth);
		int xR = IN_IMG((int)x+1, 0, m_nWidth);
		int yT = IN_IMG((int)y  , 0, m_nHeight);
		int yB = IN_IMG((int)y+1, 0, m_nHeight);

		wLT = (xR - x)*(yB - y);
		wRT = (x - xL)*(yB - y);
		wLB = (xR - x)*(y - yT);
		wRB = (x - xL)*(y - yT);

		return (wLT*m_pImageData[yT*m_nWStep + xL] + wRT*m_pImageData[yT*m_nWStep + xR] + 
				wLB*m_pImageData[yB*m_nWStep + xL] + wRB*m_pImageData[yB*m_nWStep + xR]);
	}

	BYTE BiCubicIntp(double x, double y)
	{
		int x0 = IN_IMG((int)x-1, 0, m_nWidth);
		int x1 = IN_IMG((int)x  , 0, m_nWidth);
		int x2 = IN_IMG((int)x+1, 0, m_nWidth);
		int x3 = IN_IMG((int)x+2, 0, m_nWidth);

		int y0 = IN_IMG((int)y-1, 0, m_nHeight);
		int y1 = IN_IMG((int)y  , 0, m_nHeight);
		int y2 = IN_IMG((int)y+1, 0, m_nHeight);
		int y3 = IN_IMG((int)y+2, 0, m_nHeight);

		BYTE p00 = m_pImageData[y0*m_nWStep+x0];
		BYTE p01 = m_pImageData[y1*m_nWStep+x0];
		BYTE p02 = m_pImageData[y2*m_nWStep+x0];
		BYTE p03 = m_pImageData[y3*m_nWStep+x0];

		BYTE p10 = m_pImageData[y0*m_nWStep+x1];
		BYTE p11 = m_pImageData[y1*m_nWStep+x1];
		BYTE p12 = m_pImageData[y2*m_nWStep+x1];
		BYTE p13 = m_pImageData[y3*m_nWStep+x1];

		BYTE p20 = m_pImageData[y0*m_nWStep+x2];
		BYTE p21 = m_pImageData[y1*m_nWStep+x2];
		BYTE p22 = m_pImageData[y2*m_nWStep+x2];
		BYTE p23 = m_pImageData[y3*m_nWStep+x2];

		BYTE p30 = m_pImageData[y0*m_nWStep+x3];
		BYTE p31 = m_pImageData[y1*m_nWStep+x3];
		BYTE p32 = m_pImageData[y2*m_nWStep+x3];
		BYTE p33 = m_pImageData[y3*m_nWStep+x3];

		double f00 = p11;
		double f10 = p21;
		double f01 = p12;
		double f11 = p22;
		double fx00 = (p21-p01)/2.0;
		double fx10 = (p31-p11)/2.0;
		double fx01 = (p22-p02)/2.0;
		double fx11 = (p32-p12)/2.0;
		double fy00 = (p12-p10)/2.0;
		double fy10 = (p22-p20)/2.0;
		double fy01 = (p13-p11)/2.0;
		double fy11 = (p23-p21)/2.0;
		double fxy00 = (p22-p02-p20+p00)/4.0;
		double fxy10 = (p32-p12-p30+p10)/4.0;
		double fxy01 = (p23-p03-p21+p01)/4.0;
		double fxy11 = (p33-p13-p31+p11)/4.0;

		double a00 = f00;
		double a10 = fx00;
		double a20 = -3*f00 + 3*f10 -2*fx00 - fx10;
		double a30 =  2*f00 - 2*f10 +  fx00 + fx10;
		double a01 = fy00;
		double a11 = fxy00;
		double a21 = -3*fy00 + 3*fy10 - 2*fxy00 - fxy10;
		double a31 =  2*fy00 - 2*fy10 +   fxy00 + fxy10;
		double a02 = -3*f00 + 3*f01 - 2*fy00 - fy01;
		double a12 = -3*fx00 + 3*fx01 - 2*fxy00 - fxy01;
		double a22 =  9*f00 - 9*f10 - 9*f01 + 9*f11 + 6*fx00 + 3*fx10 - 6*fx01 - 3*fx11 + 6*fy00 - 6*fy10 + 3*fy01 - 3*fy11 + 4*fxy00 + 2*fxy10 + 2*fxy01 + fxy11;
		double a32 = -6*f00 + 6*f10 + 6*f01 - 6*f11 - 3*fx00 - 3*fx10 + 3*fx01 + 3*fx11 - 4*fy00 + 4*fy10 - 2*fy01 + 2*fy11 - 2*fxy00 - 2*fxy10 -   fxy01 - fxy11;
		double a03 = 2*f00 - 2*f01 + fy00 + fy01;
		double a13 = 2*fx00 - 2*fx01 + fxy00 + fxy01;
		double a23 = -6*f00 + 6*f10 + 6*f01 - 6*f11 - 4*fx00 - 2*fx10 + 4*fx01 + 2*fx11 - 3*fy00 + 3*fy10 - 3*fy01 + 3*fy11 - 2*fxy00 - fxy10 - 2*fxy01 - fxy11;
		double a33 =  4*f00 - 4*f10 - 4*f01 + 4*f11 + 2*fx00 + 2*fx10 - 2*fx01 - 2*fx11 + 2*fy00 - 2*fy10 + 2*fy01 - 2*fy11 + fxy00 + fxy10 + fxy01 + fxy11;

		double dx = x - x1;
		double dx2 = dx*dx;
		double dx3 = dx2*dx;
		double dy = y - y1;
		double dy2 = dy*dy;
		double dy3 = dy2*dy;

		return	CLIP(
				a00		+ a01*dy	 + a02*dy2	   + a03*dy3 +
				a10*dx	+ a11*dx*dy	 + a12*dx*dy2  + a13*dx*dy3 +
				a20*dx2	+ a21*dx2*dy + a22*dx2*dy2 + a23*dx2*dy3 +
				a30*dx3	+ a31*dx3*dy + a32*dx3*dy2 + a33*dx3*dy3);
	}

	BYTE CubicConv(double x, double y)
	{
		int px[4], py[4];
		px[0] = IN_IMG((int)x-1, 0, m_nWidth);
		px[1] = IN_IMG((int)x  , 0, m_nWidth);
		px[2] = IN_IMG((int)x+1, 0, m_nWidth);
		px[3] = IN_IMG((int)x+2, 0, m_nWidth);
		py[0] = IN_IMG((int)y-1, 0, m_nHeight);
		py[1] = IN_IMG((int)y  , 0, m_nHeight);
		py[2] = IN_IMG((int)y+1, 0, m_nHeight);
		py[3] = IN_IMG((int)y+2, 0, m_nHeight);

		double dx = x - px[1];
		double dy = y - py[1];
		double dx2 = dx*dx;
		double dx3 = dx2*dx;
		double dy2 = dy*dy;
		double dy3 = dy2*dy;

		BYTE   p0, p1, p2, p3;  // �� ���� ��ǥ�� �ȼ� ��
		double c1, c2, c3, c4;  // �� ���� ��ǥ�� ����ġ
		double C[4];			// ���� ���� ���� ��� 

		for (int i=0 ; i<4 ; i++)
		{
			p0 = m_pImageData[py[i]*m_nWStep + px[0]];
			p1 = m_pImageData[py[i]*m_nWStep + px[1]];
			p2 = m_pImageData[py[i]*m_nWStep + px[2]];
			p3 = m_pImageData[py[i]*m_nWStep + px[3]];
			c1 = p1;
			c2 = -p0 + p2;
			c3 = 2*(p0-p1) + p2 - p3;
			c4 = -p0 + p1 - p2 + p3;
			C[i] = c1 + c2*dx + c3*dx2 + c4*dx3;
		}

		c1 = C[1];
		c2 = -C[0] + C[2];
		c3 = 2*(C[0]-C[1]) + C[2] - C[3];
		c4 = -C[0] + C[1] - C[2] + C[3];
		return CLIP(c1 + c2*dy + c3*dy2 + c4*dy3);
	}

	int GetChannel()	const { return m_nChannels;  }
	int GetHeight()		const { return m_nHeight;    }
	int GetWidth()		const { return m_nWidth;	 }
	int GetWStep()		const { return m_nWStep;	 }
	T*  GetPtr(int r=0, int c=0)	const { return m_pImageData + r*m_nWStep + c; }
	// jslee
	CString GetPath()	const { return m_strPathName; }

	BYTE* GetBitmapData(int sliceId /*= -1*/)
	{
		if (_bitmapData == NULL)
		{
			return NULL;
		}
		if (sliceId < 0 || sliceId >= _numberOfSlices)
		{
			sliceId = _currentSlice;
		}
		if (_bitmapData[sliceId] == NULL)
		{
			_bitmapData[sliceId] = new BYTE[_dim[0] * _dim[1]];
			memset(_bitmapData[sliceId], 0, sizeof(BYTE) * _dim[0] * _dim[1]);
		}
		return _bitmapData[sliceId];
	}
	// ����ũ �����͸� �����´�.
	BYTE* GetMaskData(int sliceId)
	{
		if (_wholeMask == NULL)
		{
			return NULL;
		}

		if (sliceId < 0 || sliceId >= _numberOfSlices)
		{
			sliceId = _currentSlice;
		}

		return (_wholeMask + _imageSize * sliceId);
	}



protected:
	int		m_nChannels;	// ä�� ��
	int		m_nHeight;		// ���� �ȼ� ��
	int		m_nWidth;		// ���� �ȼ� ��
	int		m_nWStep;		// ��� ������ ���� ��
	T*		m_pImageData;	// �ȼ� �迭 ������

public:
	CString	m_strPathName;	// ���� ���� ��� ȹ��
	

};

typedef CMyImage <BYTE  > CByteImage;
typedef CMyImage <int   > CIntImage;
typedef CMyImage <float > CFloatImage;
typedef CMyImage <double> CDoubleImage;

