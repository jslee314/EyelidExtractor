
// FeatureExtractorDlg.cpp : 구현 파일

#include "stdafx.h"
#include "FeatureExtractor.h"
#include "FeatureExtractorDlg.h"
#include "afxdialogex.h"

#include "imageSrc\Features.h"
#include "imageSrc\Segmentation.h"
#include "imageSrc\MorphologyFilter.h"
#include "imageSrc\EyelidSeg.h"
#include "imageSrc\Graphcut.h"

#include "imageSrc\MyImageFunc.h"				// 이미지 사칙연산등
#include "imageSrc\ImageFrameWndManager.h"		// 프레임윈도우, 마이 이미지 파일 포함되어 있음
#include "imageSrc\LoadImageFromFileDialog.h"	// 입력영상 읽을 때 대화상자 띄우는 것
#include "CannyThrsDlg.h"
#include "HoughParam.h"
#include <math.h>
#include <string.h>
#include <algorithm>
#include <cmath>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFeatureExtractorDlg 대화 상자

CFeatureExtractorDlg::CFeatureExtractorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFeatureExtractorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFeatureExtractorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// 
}

BEGIN_MESSAGE_MAP(CFeatureExtractorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD_INPUT, &CFeatureExtractorDlg::OnBnClickedButtonLoadInput)
	ON_BN_CLICKED(IDC_BUTTON_HOUGH, &CFeatureExtractorDlg::OnBnClickedButtonHough)
	ON_BN_CLICKED(IDC_BUTTON_CANNY, &CFeatureExtractorDlg::OnBnClickedButtonCanny)
	ON_BN_CLICKED(IDC_BUTTON_SOBEL, &CFeatureExtractorDlg::OnBnClickedButtonSobel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_IMAGE, &CFeatureExtractorDlg::OnBnClickedButtonSaveImage)
	
	ON_BN_CLICKED(IDC_BUTTON_BINARIZATION, &CFeatureExtractorDlg::OnBnClickedButtonBinarization)
	ON_EN_CHANGE(IDC_EDIT_THRES, &CFeatureExtractorDlg::OnEnChangeEditThres)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_THRES, &CFeatureExtractorDlg::OnNMCustomdrawSliderThres)
	ON_BN_CLICKED(IDC_BUTTON_Eyelid, &CFeatureExtractorDlg::OnBnClickedButtonEyelid)
	ON_BN_CLICKED(IDC_BUTTON_DILATION, &CFeatureExtractorDlg::OnBnClickedButtonDilation)
	ON_BN_CLICKED(IDC_BUTTON_ERSION, &CFeatureExtractorDlg::OnBnClickedButtonErsion)
	ON_BN_CLICKED(IDC_BUTTON_Pupil, &CFeatureExtractorDlg::OnBnClickedButtonPupil)
	ON_BN_CLICKED(IDC_BUTTON_Iris, &CFeatureExtractorDlg::OnBnClickedButtonIris)
	ON_BN_CLICKED(IDC_BUTTON_Equli, &CFeatureExtractorDlg::OnBnClickedButtonEquli)
	ON_BN_CLICKED(IDC_BUTTON_Parabola, &CFeatureExtractorDlg::OnBnClickedButtonParabola)
	ON_BN_CLICKED(IDC_PostDeepLearning, &CFeatureExtractorDlg::OnBnClickedPostdeeplearning)
	ON_BN_CLICKED(IDC_BUTTON_Roi, &CFeatureExtractorDlg::OnBnClickedButtonRoi)
END_MESSAGE_MAP()


// CFeatureExtractorDlg 메시지 처리기

BOOL CFeatureExtractorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_sliderThres.SetRange(0, 255);
	m_sliderThres.SetPos(m_nThreshold);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CFeatureExtractorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFeatureExtractorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFeatureExtractorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}






// "Sobel" 버튼
void CFeatureExtractorDlg::OnBnClickedButtonSobel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_imageIn.GetChannel() == 3)
		SobelEdge(RGB2Gray(m_imageIn), m_imageOut);
	else
		SobelEdge(m_imageIn, m_imageOut);
	ShowImage(m_imageOut, "Sobel 검출기");
}

// "Canny" 버튼
void CFeatureExtractorDlg::OnBnClickedButtonCanny()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CCannyThrsDlg cannyDlg;
	if (cannyDlg.DoModal() == IDOK)
	{
		UpdateData(TRUE);
		CannyEdge(RGB2Gray(m_imageIn), m_imageOut, cannyDlg.m_nThresholdHi, cannyDlg.m_nThresholdLo);
		ShowImage(m_imageOut, "캐니 경계선 검출기");
	}
}


// "허프 변환" 버튼
void CFeatureExtractorDlg::OnBnClickedButtonHough()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CByteImage imageEdge;
	CHoughParam houghDlg;
	if (houghDlg.DoModal() == IDOK)
	{
		UpdateData(TRUE);
		if (houghDlg.m_nEdgeMethod == 0)
		{
			if (m_imageIn.GetChannel() == 3)
				SobelEdge(RGB2Gray(m_imageIn), imageEdge);
			else
				SobelEdge(m_imageIn, imageEdge);
		}
		else
		{
			if (m_imageIn.GetChannel() == 3)
				CannyEdge(RGB2Gray(m_imageIn), imageEdge, 60, 30);
			else
				CannyEdge(m_imageIn, imageEdge, 60, 30);
		}
		//		ShowImage(imageEdge, "경계선 영상");

		double arrRho[100];
		double arrTheta[100];
		int nLine = HoughLines(imageEdge, houghDlg.m_nTNum, houghDlg.m_nTVal, houghDlg.m_dResTheta, 100, arrRho, arrTheta);

		m_imageOut = m_imageIn;
		// 직선 그리기
		for (int i = 0; i<nLine; i++)
		{
			if (arrTheta[i] == 90) // 수직선
			{
				DrawLine(m_imageOut, arrRho[i], 0, arrRho[i], m_imageOut.GetHeight() - 1, 255, 0, 0);
			}
			else
			{
				int x1 = 0;
				int y1 = (int)(arrRho[i] / cos(arrTheta[i] * M_PI / 180) + 0.5);
				int x2 = m_imageOut.GetWidth() - 1;
				int y2 = (int)((arrRho[i] - x2*sin(arrTheta[i] * M_PI / 180)) / cos(arrTheta[i] * M_PI / 180) + 0.5);
				DrawLine(m_imageOut, x1, y1, x2, y2, 255, 0, 0);
			}
		}
		ShowImage(m_imageOut, "직선 검출 결과");
	}
}



// "입력 영상" 버튼
void CFeatureExtractorDlg::OnBnClickedButtonLoadInput()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_imageIn = LoadImageFromDialog();		// 파일 대화상자를 열어서 입력영상 받아온다.
	ShowImage(m_imageIn, "입력 영상");
}

// <결과 저장> 버튼
void CFeatureExtractorDlg::OnBnClickedButtonSaveImage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_imageOut.SaveImage("Result.bmp");

	/*
	CString strPath_In = m_imageIn.m_strPathName;
	char *strPath_Out = strPath_In.GetBuffer(strPath_In.GetLength());
	const char *result = "result.bmp";
	strncat_s(strPath_Out, sizeof(strPath_Out), result, sizeof(result));
	m_imageOut.SaveImage(strPath_Out);
	*/
}




// 영상 분할----------------------------

// <이진화> 버튼
void CFeatureExtractorDlg::OnBnClickedButtonBinarization()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_imageIn.IsEmpty())
	{
		AfxMessageBox("입력 영상이 없습니다.");
		return;
	}
	m_imageOut = CByteImage(m_imageIn.GetWidth(), m_imageIn.GetHeight());

	if (m_imageIn.GetChannel() != 1)	// 컬러 영상
		Binarization(RGB2Gray(m_imageIn), m_imageOut, m_nThreshold, 255);
	else							// 회색조 영상
		Binarization(m_imageIn, m_imageOut, m_nThreshold, 255);

	ShowImage(m_imageOut, "결과 영상");

}


void CFeatureExtractorDlg::OnEnChangeEditThres()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	// 하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CFeatureExtractorDlg::OnNMCustomdrawSliderThres(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CFeatureExtractorDlg::OnBnClickedButtonDilation()
{

	//m_imageOut = m_imageIn;


	CByteImage maskDilate(5, 5);	// 팽창 마스크
	maskDilate.SetConstValue(255);

	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	Dilate(m_imageTmp, maskDilate, m_imageOut);
	ShowImage(m_imageOut, "팽창 영상");
}


void CFeatureExtractorDlg::OnBnClickedButtonErsion()
{

	m_imageOut = m_imageIn;


	CByteImage maskErode(5, 5);		// 침식 마스크
	maskErode.SetConstValue(255);

	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	Erode(m_imageTmp, maskErode, m_imageOut);
	ShowImage(m_imageOut, "침식 영상");
}


void CFeatureExtractorDlg::_MeanFiltering()
{
	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();
	int nChnnl = m_imageIn.GetChannel();
	int nWStep = m_imageIn.GetWStep();
	int nHalf = m_nMeanSize / 2;

	BYTE* pIn = m_imageIn.GetPtr();
	BYTE* pOut = m_imageOut.GetPtr();

	int r, c, l;
	for (r = 0; r<nHeight; r++) // 행 이동
	{
		for (c = 0; c<nWidth; c++) // 열 이동
		{
			for (l = 0; l<nChnnl; l++) // 채널 이동
			{
				int nSum = 0;  // 픽셀 값의 합
				int nCnt = 0; // 픽셀 수
				for (int y = -nHalf; y <= nHalf; y++)
				{
					for (int x = -nHalf; x <= nHalf; x++)
					{
						int px = c + x;
						int py = r + y;

						if (px >= 0 && px<nWidth && py >= 0 && py<nHeight)
						{
							nSum += pIn[nWStep*py + nChnnl*px + l];
							nCnt++;
						}
					}
				}
				pOut[nWStep*r + nChnnl*c + l] = (BYTE)(nSum / (double)nCnt);
			} // 채널 이동 끝
		} // 열 이동 끝
	} // 행 이동 끝

	ShowImage(m_imageOut, "Image");
}

void CFeatureExtractorDlg::_GaussianFiltering(){

	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();
	int nChnnl = m_imageIn.GetChannel();
	int nWStep = m_imageIn.GetWStep();

	BYTE* pIn = m_imageIn.GetPtr();
	BYTE* pOut = m_imageOut.GetPtr();

	// 1차원 가우스 마스크 생성
	int nHalf = max((m_dGaussian * 6 - 1) / 2, 1);
	int nMeanSize = nHalf * 2 + 1;
	for (int n = 0; n <= nHalf; n++)
	{
		m_bufGss[nHalf - n] = m_bufGss[nHalf + n]
			= exp(-n*n / (2 * m_dGaussian*m_dGaussian));
	}

	int r, c, l;
	CDoubleImage tmpConv(nWidth, nHeight, nChnnl);
	double* pTmp = tmpConv.GetPtr();

	// 가로 방향 회선
	for (r = 0; r<nHeight; r++) // 행 이동
	{
		for (c = 0; c<nWidth; c++) // 열 이동
		{
			for (l = 0; l<nChnnl; l++) // 채널 이동
			{
				double dSum = 0; // (마스크*픽셀) 값의 합
				double dGss = 0; // 마스크 값의 합
				for (int n = -nHalf; n <= nHalf; n++)
				{
					int px = c + n;

					if (px >= 0 && px<nWidth)
					{
						dSum += (pIn[nWStep*r + nChnnl*px + l] * m_bufGss[nHalf + n]);
						dGss += m_bufGss[nHalf + n];
					}
				}
				pTmp[nWStep*r + nChnnl*c + l] = dSum / dGss;
			} // 채널 이동 끝
		} // 열 이동 끝
	} // 행 이동 끝

	  // 세로 방향 회선
	for (r = 0; r<nHeight; r++) // 행 이동
	{
		for (c = 0; c<nWidth; c++) // 열 이동
		{
			for (l = 0; l<nChnnl; l++) // 채널 이동
			{
				double dSum = 0; // 픽셀 값의 합
				double dGss = 0; // 마스크 값의 합
				for (int n = -nHalf; n <= nHalf; n++)
				{
					int py = r + n;

					if (py >= 0 && py<nHeight)
					{
						int absN = abs(n);
						dSum += pTmp[nWStep*py + nChnnl*c + l] * m_bufGss[nHalf + n];
						dGss += m_bufGss[nHalf + n];
					}
				}
				pOut[nWStep*r + nChnnl*c + l] = (BYTE)(dSum / dGss);
			} // 채널 이동 끝
		} // 열 이동 끝
	} // 행 이동 끝

	ShowImage(m_imageOut, "Image");
}


void CFeatureExtractorDlg::_HistogramEqualization()
{
	if (m_imageIn.GetChannel() != 1)
	{
		AfxMessageBox("회색조 영상을 입력하세요.");
		return;
	}

	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();

	memset(m_histogram, 0, 256 * sizeof(int));
	int r, c;
	for (r = 0; r<nHeight; r++){
		BYTE* pIn = m_imageIn.GetPtr(r);

		for (c = 0; c<nWidth; c++){
			m_histogram[pIn[c]]++;
		}
	}
	double dNormFactor = 255.0 / (nWidth * nHeight);
	for (int i = 0; i<256; i++){
		m_histogramCdf[i] = m_histogram[i] * dNormFactor;
	}
	for (int i = 1; i<256; i++){
		m_histogramCdf[i] = m_histogramCdf[i - 1] + m_histogramCdf[i];
	}

	for (r = 0; r<nHeight; r++){
		BYTE* pIn = m_imageIn.GetPtr(r);
		BYTE* pOut = m_imageOut.GetPtr(r);
		for (c = 0; c<nWidth; c++){
			pOut[c] = (BYTE)(m_histogramCdf[pIn[c]] + 0.5);
		}
	}

	ShowImage(m_imageOut, "히스토 그램 평활화 결과 영상");
}

void CFeatureExtractorDlg::OnBnClickedButtonEquli()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_HistogramEqualization();
}
void CFeatureExtractorDlg::OnBnClickedButtonParabola() {

 }

int CFeatureExtractorDlg::_DetectPupilGL(int MaskSize) {
	
	int minGL = 255;
	// row 행 123456
	// column ABCDEFG

	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();
	int imagesize = nWidth*nHeight;

	//int meanlist[307200];
	//for (int i=0; i < 307200; i++) {
	//	meanlist[i] = 0;
	//}
	CByteImage meanlist = CByteImage(nWidth, nHeight);
	meanlist.SetConstValue(255);
	
	BYTE* pIn = m_imageIn.GetPtr();
	BYTE* pmin = meanlist.GetPtr();
	int mean = 0;
	int num = 0;

	int numMask = MaskSize*MaskSize;
	int maskSize = MaskSize / 2;


	for (int r = maskSize; r<nHeight- maskSize; r++) {
		for (int c = maskSize; c<nWidth- maskSize; c++) {

			mean = 0;
		
			for (int r1 = -maskSize; r1 < maskSize; r1++) {
				for (int c1 = -maskSize; c1 < maskSize; c1++){

					mean = mean + pIn[(r + r1)*nWidth + (c + c1)];
				
				}
			}

			mean = (int)((int)mean / (int)(numMask));
			//printf("%d\n", mean);
			pmin[r*nWidth + c] = mean;
		}
	}

	FILE *ff = fopen("D:\ppmin.txt", "a");
	for (int r = maskSize; r < nHeight - maskSize; r++) {
		for (int c = maskSize; c < nWidth - maskSize; c++) {
			if (pmin[r*nWidth + c] < minGL) {
				minGL = pmin[r*nWidth + c];
			}
			fprintf(ff, "%d\n", pmin[r*nWidth + c]);
			//printf("%d\n", pmin[r*nWidth + c]);
		}
	}
	fclose(ff);
	printf("minGL: %d\n", minGL);
	
	return minGL;
}


void CFeatureExtractorDlg::_Crop(int cropSize ) {

	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();
	BYTE* pIn = m_imageIn.GetPtr();

	for (int r = 0; r < nHeight; r++) { // 행 이동
		for (int c = 0; c < nWidth; c++) { // 열 이동
			

		}
	}
}


void CFeatureExtractorDlg::OnBnClickedButtonPupil(){

	if (m_imageIn.IsEmpty()){
		AfxMessageBox("입력 영상이 없습니다.");
		return;
	}

	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();

	// CByteImage m_imageOut; // 1. 생성자 호출 (상속받은 CFeatureExtractorDlg 클래스에서 이미 호출함)
	m_imageOut  = CByteImage(nWidth, nHeight);
	m_imageTmp  = CByteImage(nWidth, nHeight);
	m_imagePupil = CByteImage(nWidth, nHeight);

	// 1. 이진화 
	int maskSize = 20;
	m_nThreshold = _DetectPupilGL(maskSize);
	// printf("%d\n", m_nThreshold);
	Binarization(m_imageIn, m_imageOut, m_nThreshold, 0);
	//ShowImage(m_imageOut, "1. 이진화 결과");

	// 2. Erosion x3, Dilation x3
	CByteImage maskErode(5, 5);		// 침식 마스크
	maskErode.SetConstValue(255);
	CByteImage maskDilate(5, 5);	// 팽창 마스크
	maskDilate.SetConstValue(255);

	int morpNum = 1;
	for (int i = 0; i < morpNum; i++) {
		m_imageTmp = m_imageOut;		// 임시 입력 영상
		m_imageOut.SetConstValue(0);	// 결과 영상 초기화
		Dilate(m_imageTmp, maskDilate, m_imageOut);
	}
	//ShowImage(m_imageOut, "2. Dilate x3, 결과");

	for (int i = 0; i < morpNum; i++) {
		m_imageTmp = m_imageOut;		// 임시 입력 영상
		m_imageOut.SetConstValue(0);	// 결과 영상 초기화
		Erode(m_imageTmp, maskErode, m_imageOut);
	}
//	ShowImage(m_imageOut, "2. Erosion x3, 결과");

	// 4. CCL
	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	CCL_four(m_imageTmp, m_imageOut, nWidth, nHeight, 255, 60);
	//ShowImage(m_imageOut, "4. CCL 결과");

	// 5. Contour 추출
	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	ContourExtraction(m_imageTmp, m_imageOut);
	//ShowImage(m_imageOut, "5. Contour 결과");

	// 6. Convex Hull 추출
	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	convex_hull(m_imageTmp, m_imageOut);
//	ShowImage(m_imageOut, "6. Convex Hull 결과");

	// 7. CCL 
	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	CCL_four(m_imageTmp, m_imageOut, nWidth, nHeight, 0, 60);
//	ShowImage(m_imageOut, "7. CCL  결과");

	// pupil에 저장 ------------------------------
	BYTE* pOut = m_imageOut.GetPtr();
	BYTE* pPupil = m_imagePupil.GetPtr();

	for (int r = 0; r<nHeight; r++){
		for (int c = 0; c<nWidth; c++){
			int index = r*nWidth + c;
			if (pOut[index] == OBJECT){
				pPupil[index] = OBJECT;
			}
			if (pOut[index] == BACKGROUND){
				pPupil[index] = BACKGROUND;
			}
		}
	}
	ShowImage(m_imagePupil, "m_imagePupil 추출 결과");

	m_imageTmp = m_imageOut;
	BYTE* pIn = m_imageIn.GetPtr();
	for (int r = 0; r<nHeight; r++){
		for (int c = 0; c<nWidth; c++){

			int index = r*nWidth + c;
			if (pPupil[index] == BACKGROUND) {
				pOut[index] = pIn[index];
			}
			if (pPupil[index] == OBJECT) {
				pOut[index] = 150;
			}

		}
	}
	ShowImage(m_imageOut, "imagePupil 추출 결과");

	/////////////////////////////////////////////////////////////////
	 
	Roi r;
	r.stX = nWidth;	r.stY = nHeight;	r.endX = 0;	r.endY = 0;
	
//	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	roi = Detect_ROI(m_imageTmp, m_imageOut, m_imageIn, r);
	printf("(%d,%d) (%d,%d)\n", roi.stX, roi.stY, roi.endX, roi.endY);

	int rHeight = ((int)((roi.endY - roi.stY) / 2));
	int rWidth = ((int)((roi.endX - roi.stX) / 2));

	//Eye_factor f; 동공 중심 및 반지름
	f.centerX = roi.endX - rWidth;
	f.centerY = roi.endY - rHeight;
	f.pupilR = roi.endX - f.centerX;
	printf( "center: (%d, %d)\nr: %d \n\n", f.centerX, f.centerY, f.pupilR );

	// Roi roi;	
	roi.stX = 0;
	roi.endX = 640;
	roi.stY = f.centerY - 150;
	roi.endY = f.centerY + 150;
	printf("start(%d,%d), end(%d,%d)\n", roi.stX, roi.stY, roi.endX, roi.endY);


}




void CFeatureExtractorDlg::OnBnClickedButtonIris()
{
	if (m_imageIn.IsEmpty()) {
		AfxMessageBox("홍채 영상이 없습니다.");
		return;
	}

	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();

	m_imageOut	= CByteImage(nWidth, nHeight);
	m_imageTmp	= CByteImage(nWidth, nHeight);
	m_imagePupil= CByteImage(nWidth, nHeight);
	m_imageIris = CByteImage(nWidth, nHeight);

	// 1. 이진화 or Otzu_Threshold
	m_imageTmp = m_imageIn;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	m_nThreshold = 110;
	Binarization(m_imageTmp, m_imageOut, m_nThreshold, 255);

	// 2. CCL
	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	CCL_four(m_imageTmp, m_imageOut, nWidth, nHeight, 0, 0);

	// 3. Erosion x3, Dilation x3
	CByteImage maskErode(5, 5);		// 침식 마스크
	maskErode.SetConstValue(255);

	CByteImage maskDilate(5, 5);	// 팽창 마스크
	maskDilate.SetConstValue(255);

	int morpNum = 4;
	for (int i = 0; i < morpNum; i++) {
		m_imageTmp = m_imageOut;		// 임시 입력 영상
		m_imageOut.SetConstValue(0);	// 결과 영상 초기화
		Erode(m_imageTmp, maskErode, m_imageOut);
	}
	for (int i = 0; i < morpNum; i++) {
		m_imageTmp = m_imageOut;		// 임시 입력 영상
		m_imageOut.SetConstValue(0);	// 결과 영상 초기화
		Dilate(m_imageTmp, maskDilate, m_imageOut);
	}

	// CCL
	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	CCL_four(m_imageTmp, m_imageOut, nWidth, nHeight, BACKGROUND,0);
	
	// Contour 추출
	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	ContourExtraction(m_imageTmp, m_imageOut);
	//ShowImage(m_imageOut, "Contour Extraction 결과");

	// Convex Hull 추출
	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	convex_hull(m_imageTmp, m_imageOut);
	//ShowImage(m_imageOut, "convex_hull Extraction 결과");

	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	CCL_four(m_imageTmp, m_imageOut, nWidth, nHeight, 0, 0);


	CByteImage maskErode1(1, 7);		// 침식 마스크
	maskErode1.SetConstValue(255);
	for (int i = 0; i < 4; i++) {
		m_imageTmp = m_imageOut;		// 임시 입력 영상
		m_imageOut.SetConstValue(0);	// 결과 영상 초기화
		Erode(m_imageTmp, maskErode1, m_imageOut);
	}
	CByteImage maskDilate1(7, 1);	// 팽창 마스크
	maskDilate1.SetConstValue(255);
	for (int i = 0; i < 4; i++) {
		m_imageTmp = m_imageOut;		// 임시 입력 영상
		m_imageOut.SetConstValue(0);	// 결과 영상 초기화
		Dilate(m_imageTmp, maskDilate1, m_imageOut);
	}

	// Iris에 저장 ------------------------------
	 m_imageIris= m_imageOut;
	//ShowImage(m_imageIris, "m_imageIris 결과");

	BYTE* pOut = m_imageOut.GetPtr();
	BYTE* pIris = m_imageIris.GetPtr();
	BYTE* pIn = m_imageIn.GetPtr();

	for (int r = 0; r<nHeight; r++){
		for (int c = 0; c<nWidth; c++){
			int index = r*nWidth + c;
			if (pIris[index] == BACKGROUND) {
				pOut[index] = pIn[index];
			}
			if (pIris[index] == OBJECT) {
				pOut[index] = BACKGROUND;
			}
		}
	}
	ShowImage(m_imageOut, "imageIris 추출 결과");	
	
}


void CFeatureExtractorDlg::OnBnClickedButtonRoi()
{
	if (m_imageIn.IsEmpty()) {
		AfxMessageBox("입력 영상이 없습니다.");
		return;
	}
	if (m_imagePupil.IsEmpty()) {
		AfxMessageBox("홍채 추출 먼저 수행하세요");
		return;
	}
	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();

	m_imageOut = CByteImage(nWidth, nHeight);
	m_imageOut.SetConstValue(0);

	m_imageEyelid_upper = CByteImage(nWidth, nHeight);
	m_imageEyelid_under = CByteImage(nWidth, nHeight);
	m_imageEyelid_upper.SetConstValue(0);
	m_imageEyelid_under.SetConstValue(0);

	upper.stX = roi.stX;	upper.stY = roi.stY;	upper.endX = roi.endX;	upper.endY = f.centerY;
	printf("upper: start(%d,%d), end(%d,%d)\n", upper.stX, upper.stY, upper.endX, upper.endY);

	under.stX = roi.stX;	under.stY = f.centerY;	under.endX = roi.endX;	under.endY = roi.endY;
	printf("under: start(%d,%d), end(%d,%d)\n", under.stX, under.stY, under.endX, under.endY);

	// Roi 영역 crop - up
	Crop_ROI(m_imageIn, m_imageEyelid_upper, upper);
	ShowImage(m_imageEyelid_upper, "m_imageEyelid_upper 추출 결과");

	// Roi 영역 crop - under
	Crop_ROI(m_imageIn, m_imageEyelid_under, under);
	ShowImage(m_imageEyelid_under, "m_imageEyelid_under 추출 결과");




}


void CFeatureExtractorDlg::OnBnClickedButtonEyelid(){
	if (m_imagePupil.IsEmpty()) {
		AfxMessageBox("동공 추출 먼저 수행하세요");
		return;
	}
	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();
	m_imageEyelid = CByteImage(nWidth, nHeight);
	m_imageOut = CByteImage(nWidth, nHeight);
	CByteImage m_imageOut_under = CByteImage(nWidth, nHeight);

	m_imageOut.SetConstValue(0);
	m_imageOut_under.SetConstValue(0);

	upper_l.stX = upper.stX;		upper_l.stY = upper.stY;	upper_l.endX = f.centerX;		upper_l.endY = f.centerY;
	upper_r.stX = f.centerX;		upper_r.stY = upper.stY;	upper_r.endX = upper.endX;		upper_r.endY = upper.endY;
	
	under_l.stX	= under.stX;		under_l.stY	= under.stY;	under_l.endX = f.centerX;		under_l.endY = under.endY;
	under_r.stX = f.centerX;		under_r.stY = under.stY;	under_r.endX = under.endX;		under_r.endY = under.endY;

	// 1)  4등분 하여 분할
	_EdgeDetectionInRoi(m_imageOut, upper_l);
	//ShowImage(m_imageOut, "upper_l 추출 결과");
	_EdgeDetectionInRoi(m_imageOut, upper_r);
	//ShowImage(m_imageOut, "upper_r 추출 결과");
	_EdgeDetectionInRoi(m_imageOut, under_l);
	//ShowImage(m_imageOut, "under_l 추출 결과");
	_EdgeDetectionInRoi(m_imageOut, under_r);


	BYTE *pIn = m_imageIn.GetPtr();
	BYTE *pOut = m_imageOut.GetPtr();
	BYTE *pOut_under = m_imageOut_under.GetPtr();
	BYTE * pEyelid = m_imageEyelid.GetPtr();

	for (int r = 0; r<nHeight; r++) {
		for (int c = 0; c<nWidth; c++) {
			if (pOut[r*nWidth + c] == BACKGROUND) {
				pEyelid[r*nWidth + c] = pIn[r*nWidth + c];
			}
			if (pOut[r*nWidth + c] == OBJECT) {
				pEyelid[r*nWidth + c] = BACKGROUND;
			}
		}
	}
	ShowImage(m_imageEyelid, "under_r 추출 결과");
	m_imageEyelid.SetConstValue(0);

	// 2) 비어있는 부분 roi로 잡아서 분할
	_EdgeDetectionInRoi_Auto(m_imageOut, upper);
	_EdgeDetectionInRoi_Auto(m_imageOut, upper);
	_EdgeDetectionInRoi_Auto(m_imageOut, upper);
	
	_EdgeDetectionInRoi_Auto(m_imageOut, under);
	_EdgeDetectionInRoi_Auto(m_imageOut, under);
	_EdgeDetectionInRoi_Auto(m_imageOut, under);


	for (int r = 0; r<nHeight; r++) {
		for (int c = 0; c<nWidth; c++) {
			if (pOut[r*nWidth + c] == BACKGROUND) {
				pEyelid[r*nWidth + c] = pIn[r*nWidth + c];
			}
			if (pOut[r*nWidth + c] == OBJECT) {
				pEyelid[r*nWidth + c] = BACKGROUND;
			}
		}
	}
	ShowImage(m_imageEyelid, "roi 들 추출 결과");

	parabolaComponent *parabolaCompo = new parabolaComponent[2];

	
	// 5. Parabola Detection

	m_imageTmp = m_imageOut;
	m_imageEyelid.SetConstValue(0);
	
	parabolaCompo[0].a = 150;			
	parabolaCompo[0].x0 = f.centerX;	
	parabolaCompo[0].y0 = f.centerY-100;	
	HoughParabolaDetction(m_imageTmp, m_imageEyelid, parabolaCompo[0]);
	m_imageOut = m_imageEyelid;
	m_imageEyelid.SetConstValue(0);

	parabolaCompo[1].a = -150;			
	parabolaCompo[1].x0 = f.centerX;
	parabolaCompo[1].y0 = f.centerY + 100;
	HoughParabolaDetction(m_imageTmp, m_imageEyelid, parabolaCompo[1]);
	m_imageOut_under = m_imageEyelid;

	for (int r = 0; r<nHeight; r++) {
		for (int c = 0; c<nWidth; c++) {
			if ((pOut[r*nWidth + c] == BACKGROUND) || (pOut_under[r*nWidth + c] == BACKGROUND)) {
				pEyelid[r*nWidth + c] = pIn[r*nWidth + c];
			}
			if ((pOut[r*nWidth + c] == OBJECT) || (pOut_under[r*nWidth + c] == OBJECT)) {
				pEyelid[r*nWidth + c] = BACKGROUND;
			}
		}
	}
	ShowImage(m_imageEyelid, "parabolaCompo 결과");
	
}


// 1) 4등분 영역 분할
void CFeatureExtractorDlg::_EdgeDetectionInRoi(CByteImage& imageOut, Roi s_roi) {
	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();

	CByteImage imageTmp = CByteImage(nWidth, nHeight);
	CByteImage m_imageRoi = CByteImage(nWidth, nHeight);

	// 1. Canny edge
	imageTmp = m_imageIn;

	m_imageRoi.SetConstValue(0);
	CannyEdge_Eyelid(imageTmp, m_imageRoi, m_imageIris, 5, 5, s_roi);

	// 2. Dilation
	CByteImage maskErode(3, 1);			// 침식 마스크
	maskErode.SetConstValue(OBJECT);
	CByteImage maskDilate(3, 1);		// 침식 마스크
	maskDilate.SetConstValue(OBJECT);
	int morpNum = 2;
	for (int i = 0; i < morpNum; i++) {
		imageTmp = m_imageRoi;		// 임시 입력 영상
		m_imageRoi.SetConstValue(0);	// 결과 영상 초기화
		Dilate(imageTmp, maskDilate, m_imageRoi);
	}

	// 3. CCL long
	imageTmp = m_imageRoi;		// 임시 입력 영상
	m_imageRoi.SetConstValue(0);	// 결과 영상 초기화
	CCL_eight_long(imageTmp, m_imageRoi, OBJECT);

	BYTE* pRoi = m_imageRoi.GetPtr();
	BYTE* pOut = imageOut.GetPtr();

	for (int r = 0; r<nHeight; r++) {
		for (int c = 0; c<nWidth; c++) {
			int index = r*nWidth + c;
			if (pRoi[index] == OBJECT) {
				pOut[index] = OBJECT;
			}
		}
	}

	/*
	// 5. Parabola Detection
	imageTmp = m_imageRoi;
	m_imageRoi.SetConstValue(0);
	HoughParabolaDetction(imageTmp, m_imageRoi, parabolaCompo[1]);
	ShowImage(m_imageRoi, "parabolaCompo 결과");
	*/

}
// 2) 자동  영역 분할 _upper(roi +20 -20)
void CFeatureExtractorDlg::_EdgeDetectionInRoi_Auto(CByteImage& m_imageRoi, Roi s_roi) {

	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();
	Roi *r = new Roi[50];
	for (int a = 0; a < 50; a++) {
		r[a].stX = 0;		r[a].stY = 0;		r[a].endX = 0;		r[a].endY = 0;
	}
	int rNum = 0;

	// 2-0) 비어있는 부분 찾아 list에 추가(비어있는 부분은i가 0, 채워있는 부분은 i가 1)
	//		그후 roi r 배열에 각 부분의 시작점, 끝점 저장
	rNum = _EmptylineRoi(m_imageRoi, s_roi, r);
	int stlabel = 1;
	for (int n = 1; n<rNum; n++) {
		// 2-1) roi 범위가 작으면(x거리가 20픽셀 이하)면 직선으로 잇는다
		if ((r[n].endX - r[n].stX) < 40) {
			printf("%d:line\n", n);
			DrawLine(m_imageOut, r[n].stX, r[n].stY, r[n].endX, r[n].endY, OBJECT);
			//ShowImage(m_imageRoi, "line");

		}
		// 2-2) roi 범위가 어느정도 크다면 roi 내에서 영상정보를 통해 라인을 찾는다.
		else {
			printf("%d:s roi\n", n);
			if (r[n].stY < r[n].endY) {
				r[n].stY = r[n].stY - 15;
				r[n].endY = r[n].endY + 15;
			}
			else {
				int temp = r[n].stY;
				r[n].stY = r[n].endY - 15;
				r[n].endY = temp + 15;
			}
			_EdgeDetectionInRoi_s(m_imageOut, r[n]);
			//ShowImage(m_imageRoi, "small roi");

		}
	}
}

// 2-0) 자동 영역 찾기 
int CFeatureExtractorDlg::_EmptylineRoi(CByteImage& m_imageRoi, Roi s_roi, Roi *r) {
	// lintLinst 구해서 Roi[50] 구하자
	int nWidth = m_imageIn.GetWidth();	int nHeight = m_imageIn.GetHeight();
	BYTE* pOut = m_imageOut.GetPtr();

	lineList list[640];
	for (int a = 0; a < 640; a++) {
		list[a].i = 0;		list[a].stL = 0;		list[a].endL = 0;
		list[a].y = f.centerY; // 처음과 끝의 roi의 y값을 동공의 중심으로 한다
	}

	// y값이 0인 경우는 해당 x 좌표에서 y 값이 없는 경우, 즉 라인이 끊긴 경우
	for (int r = s_roi.stY; r < s_roi.endY; r++) {
		for (int c = 0; c < 640; c++) {
			if (pOut[r*nWidth + c] == OBJECT) {
				list[c].i = 1;	// 라인이 있으면 0 ->1로 바꿈
				list[c].y = r;
			}
		}
	}

	// roi 지정
	int rLableSt = 1;	int rLableEnd = 1;
	int state = 0; // 1이면 lable이 없는 모드(roi 지정해야할 부분), 0이면 있는 모드
	for (int a = 1; a < nWidth - 1; a++) {
		// 11->00
		if (list[a].i == 0 && ((list[a - 1].i == 1) || (a == 1))) {
			list[a - 1].stL = rLableSt;
			r[rLableSt].stX = a - 1;
			r[rLableSt].stY = list[a - 1].y;
			rLableSt++;
			state = 1;
		}
		// 00->11
		if (list[a].i == 0 && ((list[a + 1].i == 1) || (a == 638)) && state == 1) {
			list[a + 1].endL = rLableEnd;
			r[rLableEnd].endX = a + 1;
			r[rLableEnd].endY = list[a + 1].y;
			rLableEnd++;
			state = 0;
		}
	}

	//// print roi r---------------------------------------------
	CByteImage imagePoint = CByteImage(nWidth, nHeight);
	BYTE* pPoint = imagePoint.GetPtr();
	//imagePoint.SetConstValue(BACKGROUND);
	imagePoint = m_imageIn;
	
	int x = 0;
	int y = 0;
	for (int n = 1; n < rLableEnd; n++) {
		for (int i = -2; i < 3; i++) {
			for (int j = -2; j < 3; j++) {
				x = r[n].stX;	y = r[n].stY;
				if ((x + i >= 0 && x + i < nWidth) && (y + j >= 0 && y + j < nHeight)) {
					pPoint[(y + j) * nWidth + (x + i)] = 0;
				}
				x = r[n].endX;	y = r[n].endY;
				if ((x + i >= 0 && x + i < nWidth) && (y + j >= 0 && y + j < nHeight)) {
					pPoint[(y + j) * nWidth + (x + i)] = 0;
				}
			}
		}
		printf("%d	(%d	%d)	(%d	%d)\n", n, r[n].stX, r[n].stY, r[n].endX, r[n].endY);
	}
	
	//ShowImage(imagePoint, "imagePoint of Roi");

	return rLableEnd;

}

// 2-2) 
void CFeatureExtractorDlg::_EdgeDetectionInRoi_s(CByteImage& imageOut, Roi s_roi) {
	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();

	CByteImage imageTmp = CByteImage(nWidth, nHeight);
	CByteImage m_imageRoi = CByteImage(nWidth, nHeight);
	//m_imageRoi = m_imageIn;
	//printf("canny_s:  (%d	%d)	(%d	%d)\n", s_roi.stX, s_roi.stY, s_roi.endX, s_roi.endY);
	//DrawLine(m_imageRoi, s_roi.stX, s_roi.stY, s_roi.endX, s_roi.endY, 0);
	//ShowImage(m_imageRoi, "sCanny DrawLine");

	// 1. Canny edge
	imageTmp = m_imageIn;
	m_imageRoi.SetConstValue(0);
	CannyEdge_Eyelid(imageTmp, m_imageRoi, m_imageIris, 5, 5, s_roi);
	//ShowImage(m_imageRoi, "sCanny");


	// 2. Dilation
	CByteImage maskErode(3, 1);			
	maskErode.SetConstValue(OBJECT);
	CByteImage maskDilate(3, 1);
	maskDilate.SetConstValue(OBJECT);
	int morpNum = 2;
	for (int i = 0; i < morpNum; i++) {
		imageTmp = m_imageRoi;	
		m_imageRoi.SetConstValue(0);
		Dilate(imageTmp, maskDilate, m_imageRoi);
	}
	//ShowImage(m_imageRoi, "sDilation");


	// 3. CCL long
	imageTmp = m_imageRoi;		
	m_imageRoi.SetConstValue(0);
	CCL_eight_long(imageTmp, m_imageRoi, OBJECT);
	//ShowImage(m_imageRoi, "sCCL");

	BYTE* pRoi = m_imageRoi.GetPtr();
	BYTE* pOut = imageOut.GetPtr();

	for (int r = 0; r<nHeight; r++) {
		for (int c = 0; c<nWidth; c++) {
			int index = r*nWidth + c;
			if (pRoi[index] == OBJECT) {
				pOut[index] = OBJECT;
			}
		}
	}

}





void CFeatureExtractorDlg::OnBnClickedPostdeeplearning(){

	if (m_imageIn.IsEmpty())
	{
		AfxMessageBox("입력 영상이 없습니다.");
		return;
	}
	int nWidth = m_imageIn.GetWidth();
	int nHeight = m_imageIn.GetHeight();

	m_imageOut = CByteImage(nWidth, nHeight);
	m_imageTmp = CByteImage(nWidth, nHeight);
	m_imagemid = CByteImage(nWidth, nHeight);

	BYTE* pIn  = m_imageIn.GetPtr();
	BYTE* pTmp = m_imageTmp.GetPtr();
	BYTE* pOut = m_imageOut.GetPtr();

	parabolaComponent *parabolaCompo = new parabolaComponent[1];

	// 바꾸기
	ToPixel(m_imageIn, m_imageOut);
	ShowImage(m_imageOut, "m_imageOut 추출 결과");

	// Erosion x3, Dilation x3
	CByteImage maskErode(5, 5);		// 침식 마스크
	maskErode.SetConstValue(255);

	CByteImage maskDilate(5, 5);	// 팽창 마스크
	maskDilate.SetConstValue(255);

	int morpNum = 8;
	for (int i = 0; i < morpNum; i++) {
		m_imageTmp = m_imageOut;		// 임시 입력 영상
		m_imageOut.SetConstValue(0);	// 결과 영상 초기화
		Dilate(m_imageTmp, maskDilate, m_imageOut);
	}
	ShowImage(m_imageOut, "maskDilate 추출 결과");

	// 2. CCL
	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	CCL_eight(m_imageTmp, m_imageOut, 255);

	ShowImage(m_imageOut, "CCL 추출 결과");

	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {



		}
	}
	

	/*
	for (int i = 0; i < morpNum; i++) {
		m_imageTmp = m_imageOut;		// 임시 입력 영상
		m_imageOut.SetConstValue(0);	// 결과 영상 초기화
		Erode(m_imageTmp, maskErode, m_imageOut);
	}
	ShowImage(m_imageOut, "maskErode 추출 결과");
	// 2. CCL
	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	CCL_eight(m_imageTmp, m_imageOut,  255);
	ShowImage(m_imageOut, "CCL2 추출 결과");

	// Contour 추출
	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	ContourExtraction(m_imageTmp, m_imageOut);
	ShowImage(m_imageOut, "Contour Extraction 결과");

	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	convex_hull(m_imageTmp, m_imageOut);
	ShowImage(m_imageOut, "convex_hull Extraction 결과");


	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화
	CCL_four(m_imageTmp, m_imageOut, nWidth, nHeight, 0);
	ShowImage(m_imageOut, "CCL 2추출 결과");

	for (int i = 0; i < 1; i++) {
		m_imageTmp = m_imageOut;		// 임시 입력 영상
		m_imageOut.SetConstValue(0);	// 결과 영상 초기화
		Erode(m_imageTmp, maskErode, m_imageOut);
	}

	BYTE* pMid = m_imagemid.GetPtr();

	m_imageTmp = m_imageOut;		// 임시 입력 영상
	m_imageOut.SetConstValue(0);	// 결과 영상 초기화

	for (int r = 0; r<nHeight; r++) {
		for (int c = 0; c<nWidth; c++) {
			int index = r*nWidth + c;
			if (pTmp[index] == BACKGROUND) {
				pOut[index] = pIn[index];
				pMid[index] = BACKGROUND;
			}
			if (pTmp[index] == OBJECT) {
				pOut[index] = BACKGROUND;
				pMid[index] = pIn[index];
			}
		}
	}
	ShowImage(m_imageOut, "최종 추출 결과");
	ShowImage(m_imagemid, "최종 추출 결과2");
	*/
}

