#pragma once

#include "MyImage.h"

// CImageView view

class CImageView : public CView
{
public:
	CImageView();
	virtual ~CImageView();
	BOOL CreateEx(CWnd* pWnd);
	void SetImage(const CByteImage& input);
	void SetNumClkPoints(int nPoints);
	bool GetClkPoints(int* posX, int* posY, int nPoints);

protected:
	virtual void OnDraw(CDC* pDC);

	CByteImage	m_image;
	int			m_nMouseClk;
	int			m_nClkPosX[10];
	int			m_nClkPosY[10];

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


