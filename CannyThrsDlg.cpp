// CannyThrsDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FeatureExtractor.h"
#include "CannyThrsDlg.h"
#include "afxdialogex.h"


// CCannyThrsDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCannyThrsDlg, CDialogEx)

CCannyThrsDlg::CCannyThrsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCannyThrsDlg::IDD, pParent)
	, m_nThresholdHi(60)
	, m_nThresholdLo(30)
{

}

CCannyThrsDlg::~CCannyThrsDlg()
{
}

void CCannyThrsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_T_HI, m_nThresholdHi);
	DDX_Text(pDX, IDC_EDIT_T_LO, m_nThresholdLo);
}


BEGIN_MESSAGE_MAP(CCannyThrsDlg, CDialogEx)
END_MESSAGE_MAP()


// CCannyThrsDlg 메시지 처리기입니다.
