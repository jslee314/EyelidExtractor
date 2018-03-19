// HoughParam.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FeatureExtractor.h"
#include "HoughParam.h"
#include "afxdialogex.h"


// CHoughParam 대화 상자입니다.

IMPLEMENT_DYNAMIC(CHoughParam, CDialogEx)

CHoughParam::CHoughParam(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHoughParam::IDD, pParent)
	, m_nEdgeMethod(0)
	, m_nTNum(200)
	, m_nTVal(100)
	, m_dResTheta(1.0)
{

}

CHoughParam::~CHoughParam()
{
}

void CHoughParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_SOBEL, m_nEdgeMethod);
	DDX_Text(pDX, IDC_EDIT_THR_NUM, m_nTNum);
	DDX_Text(pDX, IDC_EDIT_THR_VAL, m_nTVal);
	DDX_Text(pDX, IDC_EDIT_RES_THETA, m_dResTheta);
}


BEGIN_MESSAGE_MAP(CHoughParam, CDialogEx)
END_MESSAGE_MAP()


// CHoughParam 메시지 처리기입니다.
