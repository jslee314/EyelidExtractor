#pragma once


// CHoughParam ��ȭ �����Դϴ�.

class CHoughParam : public CDialogEx
{
	DECLARE_DYNAMIC(CHoughParam)

public:
	CHoughParam(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CHoughParam();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_HOUGH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int m_nEdgeMethod;
	int m_nTNum;
	int m_nTVal;
	double m_dResTheta;
};
