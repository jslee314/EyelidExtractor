#pragma once


// CCannyThrsDlg 대화 상자입니다.

class CCannyThrsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCannyThrsDlg)

public:
	CCannyThrsDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCannyThrsDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CANNY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nThresholdHi;
	int m_nThresholdLo;
};
