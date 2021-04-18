
// FeatureExtractorDlg.h : ��� ����
//

#pragma once

#include "imageSrc\imageIO\MyImage.h"
struct lineList {
	int i;
	int stL;
	int endL;
	int y;
};



// CFeatureExtractorDlg ��ȭ ����
class CFeatureExtractorDlg : public CDialogEx {
// �����Դϴ�.
public:
	CFeatureExtractorDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FEATUREEXTRACTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	int		m_nBrightness;
	double	m_dContrast;
	double	m_dGamma;
	int		m_nMeanSize;
	double	m_dGaussian;
	double	m_bufGss[17];
	int		m_nMedSize;
	int		m_bufMed[121];
	int		m_histogram[256];
	double	m_histogramCdf[256];
	double	m_3x3Mask[9];
	int		m_nSelMask;

	CByteImage	m_GC_ObjSeed;
	CByteImage	m_GC_bckSeed;

	CByteImage	m_imageEyelid_upper;
	CByteImage  m_imageEyelid_under;

	Roi		roi;

	Roi		tmp_roi;

	Roi		upper;
	Roi		under;
	Eye_factor f;

	//int roiNum;


	Roi upper_l;
	Roi upper_r;
	Roi upper_m;

	Roi under_l;
	Roi under_r;
	Roi under_m;

	//SNAKE


protected:
	CByteImage  m_imageIn;
	CByteImage	m_imageInEx;
	CByteImage	m_imageOut;
	CByteImage	m_imageSAVE;
	
	CByteImage  m_imagemid;
	CByteImage	m_imageTmp;

	
	CByteImage	m_imagePupil;
	CByteImage	m_imageIris;
	CByteImage	m_imageEyelid;

	int			m_nThreshold;
	CSliderCtrl m_sliderThres;
	CString     m_edit;



public:
	// �Է� ����
	afx_msg void OnBnClickedButtonLoadInput();
	// ��� ����
	afx_msg void OnBnClickedButtonSaveImage();


	// ��輱 ����
	afx_msg void OnBnClickedButtonHough();
	afx_msg void OnBnClickedButtonCanny();
	afx_msg void OnBnClickedButtonSobel();
	

	// ���� ����
	afx_msg void OnBnClickedButtonBinarization();
	afx_msg void OnEnChangeEditThres();

	afx_msg void OnNMCustomdrawSliderThres(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonEyelid();
	afx_msg void OnBnClickedButtonDilation();
	afx_msg void OnBnClickedButtonErsion();
	afx_msg void OnBnClickedButtonPupil();
	afx_msg void OnBnClickedButtonIris();


	void _MeanFiltering();
	void _GaussianFiltering();
	void _HistogramEqualization();

	void _Crop(int cropSize);
	void _EximageIn(int nThreshold);
	int  _DetectPupilGL(int maskSize);
	void _EdgeDetectionInRoi(CByteImage& m_imageRoi, Roi under);
	void _EdgeDetectionInRoi_s(CByteImage& m_imageRoi, Roi under);
	void _EdgeDetectionInRoi_Auto(CByteImage& m_imageRoi, Roi under);
	int _EmptylineRoi(CByteImage& m_imageRoi, Roi s_roi, Roi *r);
	//void computegflow(const CByteImage& imageIn, BYTE* chanel_gradient, BYTE* chanel_flow);


	afx_msg void OnBnClickedButtonEquli();
	afx_msg void OnBnClickedButtonParabola();
	afx_msg void OnBnClickedPostdeeplearning();
	afx_msg void OnBnClickedButtonRoi();
	afx_msg void OnBnClickedButtonSnake();
	afx_msg void OnBnClickedTogray();
	afx_msg void OnEnChangeEdit1();
};
