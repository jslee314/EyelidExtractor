
// FeatureExtractor.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CFeatureExtractorApp:
// �� Ŭ������ ������ ���ؼ��� FeatureExtractor.cpp�� �����Ͻʽÿ�.
//

class CFeatureExtractorApp : public CWinApp
{
public:
	CFeatureExtractorApp();

	// �������Դϴ�.
public:
	virtual BOOL InitInstance();

	// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CFeatureExtractorApp theApp;