#pragma once


// CHoughParam 대화 상자입니다.

class CHoughParam : public CDialogEx
{
	DECLARE_DYNAMIC(CHoughParam)

public:
	CHoughParam(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CHoughParam();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_HOUGH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nEdgeMethod;
	int m_nTNum;
	int m_nTVal;
	double m_dResTheta;
};
