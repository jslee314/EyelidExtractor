#include "StdAfx.h"
#include "LoadImageFromFileDialog.h"

CByteImage LoadImageFromDialog()
{
	CByteImage image;	// ��ȯ�� ����

	// ���� ��ȭ���ڿ��� BMP ������ ���ϸ� ���̵��� �ϴ� ������ ����
	char szFilter[] = "BMP File (*.BMP) | *.BMP; | All Files(*.*)|*.*||";	

	// �Է� ���� �б�
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (IDOK != dlg.DoModal()) // ��ȭ ���� ����
	{
		return image;
	}

	CString strPathName = dlg.GetPathName(); // ���� ���� ��� ȹ��

	image.LoadImage(strPathName); // ���� �б�

	image.m_strPathName = strPathName;

	

	return image;
}