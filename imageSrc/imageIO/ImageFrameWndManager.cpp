// ImageFrameWndManager.cpp : implementation file
//

#include "StdAfx.h"
#include "ImageFrameWndManager.h"
#include "ImageFrameWnd.h"

CImageFrameWndManager::CImageFrameWndManager(void)
{
}

CImageFrameWndManager::~CImageFrameWndManager(void)
{
	CloseAll();
}

void CImageFrameWndManager::Add(CImageFrameWnd* pImageWnd)
{
	m_vpImageWnd.push_back(pImageWnd);
}

CImageFrameWnd* CImageFrameWndManager::FindWnd(const char* name)
{
	CImageFrameWnd* ret = NULL;
	if (name)
	{
		for (unsigned int i=0 ; i<m_vpImageWnd.size() ; i++)
		{
			CString wName;
			m_vpImageWnd.at(i)->GetWindowText(wName);
			if (wName == name) // ��ġ�ϴ� ���ڿ� �߰�
			{
				ret = m_vpImageWnd.at(i);
				break;
			}
		}
	}
	return ret;
}

void CImageFrameWndManager::Delete(CImageFrameWnd* pImageWnd)
{
 	for (unsigned int i=0 ; i<m_vpImageWnd.size() ; i++)
	{
		if (m_vpImageWnd.at(i) == pImageWnd)
		{
			delete m_vpImageWnd.at(i);
			m_vpImageWnd.erase(m_vpImageWnd.begin()+i);
			break;
		}
	}
}

void CImageFrameWndManager::CloseAll()
{
 	for (int i=m_vpImageWnd.size()-1 ; i>=0 ; i--)
 	{
		delete m_vpImageWnd.at(i);
		m_vpImageWnd.erase(m_vpImageWnd.begin()+i);
 	}
}

CImageFrameWndManager gImageFrameWndManager;
void ShowImage(const CByteImage& input, const char *name)
{
	CImageFrameWnd *pImageWnd = gImageFrameWndManager.FindWnd(name);
	if (pImageWnd) // ���� �̸��� â �߰�
	{
		pImageWnd->GetImageView().SetImage(input);	 // ���� â�� ���� ����
		pImageWnd->GetImageView().Invalidate(false); // ȭ�� ���� �Լ� ȣ��
	}
	else // ���� �̸��� â�� ���� �� �� â�� �����ϰ� �迭�� �߰�
	{
		gImageFrameWndManager.Add(new CImageFrameWnd(input, name));
	}
}

void CloseImage(const char* name)
{
	CImageFrameWnd* pImageWnd = gImageFrameWndManager.FindWnd(name);
	gImageFrameWndManager.Delete(pImageWnd);
}

void CloseAllImages()
{
	gImageFrameWndManager.CloseAll();
}

BOOL PeekAndPump()
{
	MSG msg;
	while(::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
	{
		if(!AfxGetApp()->PumpMessage())
		{
			::PostQuitMessage(0);
			return false;
		}
	}
	return true;
}

bool GetClickPoints(const char* name, int* posX, int* posY, int nPoints)
{
	CImageFrameWnd* pWnd = gImageFrameWndManager.FindWnd(name);

	if (!pWnd) return false;

	pWnd->SetFocus();
	pWnd->GetImageView().SetNumClkPoints(nPoints);

	while (!pWnd->GetImageView().GetClkPoints(posX, posY, nPoints))
	{
		PeekAndPump();
	}
	return true;
}
