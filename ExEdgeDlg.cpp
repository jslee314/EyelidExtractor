// ExEdgeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExEdge.h"
#include "ExEdgeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExEdgeDlg dialog

CExEdgeDlg::CExEdgeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExEdgeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExEdgeDlg)
	m_nHighThreshold = 0;
	m_nLowThreshold = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExEdgeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExEdgeDlg)
	DDX_Text(pDX, IDC_T1, m_nHighThreshold);
	DDV_MinMaxInt(pDX, m_nHighThreshold, 1, 1000);
	DDX_Text(pDX, IDC_T2, m_nLowThreshold);
	DDV_MinMaxInt(pDX, m_nLowThreshold, 1, 1000);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExEdgeDlg, CDialog)
	//{{AFX_MSG_MAP(CExEdgeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SOBEL, OnSobel)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LAP, OnLap)
	ON_BN_CLICKED(IDC_CANNY, OnCanny)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExEdgeDlg message handlers

BOOL CExEdgeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_nHighThreshold = 60;
	m_nLowThreshold = 30;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CExEdgeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CExEdgeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CExEdgeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CExEdgeDlg::OnLoad() 
{
	// TODO: Add your control notification handler code here
	m_srcImage = LoadImageFromFileDialog();
	if(m_srcImage.GetCSize() != 1 && m_srcImage.GetXSize() != 0)
	{
		m_srcImage = ColorToGray(m_srcImage);
	}
	if(m_srcImage.GetXSize() != 0)
	{
		ImShow(m_srcImage, "Original Image");
	}
}

void CExEdgeDlg::OnSobel() 
{
	// TODO: Add your control notification handler code here
	m_edgeImage.Allocate(m_srcImage.GetXSize(), m_srcImage.GetYSize());

	unsigned char** ppSrcData = (unsigned char**)m_srcImage.Get2DBuffer();
	unsigned char** ppEdgeData = (unsigned char**)m_edgeImage.Get2DBuffer();

	int Gx[3][3], Gy[3][3];

	Gx[0][0] = -1; Gx[0][1] = 0; Gx[0][2] = 1;
	Gx[1][0] = -2; Gx[1][1] = 0; Gx[1][2] = 2;
	Gx[2][0] = -1; Gx[2][1] = 0; Gx[2][2] = 1;

	Gy[0][0] =  1; Gy[0][1] =  2; Gy[0][2] =  1;
	Gy[1][0] =  0; Gy[1][1] =  0; Gy[1][2] =  0;
	Gy[2][0] = -1; Gy[2][1] = -2; Gy[2][2] = -1;

	int sum, sumX, sumY;

	for(int y=0 ; y<m_srcImage.GetYSize() ; y++)
	{
		for(int x=0 ; x<m_srcImage.GetXSize() ; x++)
		{
			sum = sumX = sumY = 0;

			//image boundary
			if(x==0 || x==m_srcImage.GetXSize()-1)
			{
				sum = 0;
			}
			else if(y==0 || y==m_srcImage.GetYSize()-1)
			{
				sum = 0;
			}
			else
			{
				for(int i=-1 ; i<=1 ; i++)
				{
					for(int j=-1 ; j<=1 ; j++)
					{
						sumX = sumX + (int)(ppSrcData[y+j][x+i] * Gx[i+1][j+1]);
					}
				}

				for(i=-1 ; i<=1 ; i++)
				{
					for(int j=-1 ; j<=1 ; j++)
					{
						sumY = sumY + (int)(ppSrcData[y+j][x+i] * Gy[i+1][j+1]);
					}
				}

				sum = abs(sumX) + abs(sumY);
				
				if(sum > 255)
				{
					sum = 255;
				}
			}
			
			ppEdgeData[y][x] = 255 - (unsigned char)sum;
		}
	}

	ImShow(m_edgeImage, "result");
}

void CExEdgeDlg::OnSave() 
{
	// TODO: Add your control notification handler code here
	CFileDialog pDlg(FALSE, "*", "*.bmp", OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "∫Ò∆Æ∏  ∆ƒ¿œ(*.bmp)|*.bmp|");
	if(pDlg.DoModal() == IDOK)
	{
		m_edgeImage.SaveImage(pDlg.GetPathName());
	}
}

void CExEdgeDlg::OnLap() 
{
	// TODO: Add your control notification handler code here
	m_edgeImage.Allocate(m_srcImage.GetXSize(), m_srcImage.GetYSize());

	unsigned char** ppSrcData = (unsigned char**)m_srcImage.Get2DBuffer();
	unsigned char** ppEdgeData = (unsigned char**)m_edgeImage.Get2DBuffer();

	int MASK[5][5];

	MASK[0][0] = -1; MASK[0][1] = -1; MASK[0][2] = -1; MASK[0][3] = -1; MASK[0][4] = -1;
	MASK[1][0] = -1; MASK[1][1] = -1; MASK[1][2] = -1; MASK[1][3] = -1; MASK[1][4] = -1;
	MASK[2][0] = -1; MASK[2][1] = -1; MASK[2][2] = 24; MASK[2][3] = -1; MASK[2][4] = -1;
	MASK[3][0] = -1; MASK[3][1] = -1; MASK[3][2] = -1; MASK[3][3] = -1; MASK[3][4] = -1;
	MASK[4][0] = -1; MASK[4][1] = -1; MASK[4][2] = -1; MASK[4][3] = -1; MASK[4][4] = -1;

	int sum;

	for(int y=0 ; y<m_srcImage.GetYSize() ; y++)
	{
		for(int x=0 ; x<m_srcImage.GetXSize() ; x++)
		{
			sum = 0;

			//image boundary
			if(x<=1 || x>=m_srcImage.GetXSize()-2)
			{
				sum = 0;
			}
			else if(y<=1 || y>=m_srcImage.GetYSize()-2)
			{
				sum = 0;
			}
			else
			{
				for(int i=-2; i<=2; i++)
				{
					for(int j=-2; j<=2; j++)
					{
						sum = sum + (int)(ppSrcData[y+j][x+i] * MASK[i+2][j+2]);
					}
				}

				if(sum > 255)
				{
					sum = 255;
				}

				if(sum < 0)
				{
					sum = 0;
				}
			}
			
			ppEdgeData[y][x] = 255 - (unsigned char)sum;
		}
	}

	ImShow(m_edgeImage, "result");
}



void CExEdgeDlg::OnCanny() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_tmpImage.Allocate(m_srcImage.GetXSize(), m_srcImage.GetYSize());
	m_strengthImage.Allocate(m_srcImage.GetXSize(), m_srcImage.GetYSize());
	m_normalImage.Allocate(m_srcImage.GetXSize(), m_srcImage.GetYSize());
	m_edgeImage.Allocate(m_srcImage.GetXSize(), m_srcImage.GetYSize());

	unsigned char** ppSrcData = (unsigned char**)m_srcImage.Get2DBuffer();
	int** ppTmpData = (int**)m_tmpImage.Get2DBuffer();
	int** ppStrengthData = (int**)m_strengthImage.Get2DBuffer();
	int** ppNormalData = (int**)m_normalImage.Get2DBuffer();
	unsigned char** ppEdgeData = (unsigned char**)m_edgeImage.Get2DBuffer();

	for(int y=0 ; y<m_srcImage.GetYSize() ; y++)
	{
		for(int x=0 ; x<m_srcImage.GetXSize() ; x++)
		{
			ppEdgeData[y][x] = 255;
		}
	}

	/*
	 * Gaussian filtering
	 */
	int GAUSSIAN[5][5];

	GAUSSIAN[0][0] = 2; GAUSSIAN[0][1] = 4; GAUSSIAN[0][2] = 5; GAUSSIAN[0][3] = 4; GAUSSIAN[0][4] = 2;
	GAUSSIAN[1][0] = 4; GAUSSIAN[1][1] = 9; GAUSSIAN[1][2] = 12; GAUSSIAN[1][3] = 9; GAUSSIAN[1][4] = 4;
	GAUSSIAN[2][0] = 5; GAUSSIAN[2][1] = 12; GAUSSIAN[2][2] = 15; GAUSSIAN[2][3] = 12; GAUSSIAN[2][4] = 5;
	GAUSSIAN[3][0] = 4; GAUSSIAN[3][1] = 9; GAUSSIAN[3][2] = 12; GAUSSIAN[3][3] = 9; GAUSSIAN[3][4] = 4;
	GAUSSIAN[4][0] = 2; GAUSSIAN[4][1] = 4; GAUSSIAN[4][2] = 5; GAUSSIAN[4][3] = 4; GAUSSIAN[4][4] = 2;

	float sum;

	for(y=0 ; y<m_srcImage.GetYSize() ; y++)
	{
		for(int x=0 ; x<m_srcImage.GetXSize() ; x++)
		{
			sum = 0.0;

			//image boundary
			if(x<=1 || x>=m_srcImage.GetXSize()-2)
			{
				sum = ppSrcData[y][x];
			}
			else if(y<=1 || y>=m_srcImage.GetYSize()-2)
			{
				sum = ppSrcData[y][x];
			}
			else
			{
				for(int i=-2; i<=2; i++)
				{
					for(int j=-2; j<=2; j++)
					{
						sum = sum + ((float)ppSrcData[y+j][x+i] * (float)GAUSSIAN[i+2][j+2])/155.0;
					}
				}
			}
			
			ppTmpData[y][x] = (int)sum;
		}
	}

	ImShow(m_tmpImage, "Gaussian filtered");

	/*
	 * find the edge strength and direction
	 */
	int Gx[3][3], Gy[3][3];
	float theta;

	Gx[0][0] = -1; Gx[0][1] = 0; Gx[0][2] = 1;
	Gx[1][0] = -2; Gx[1][1] = 0; Gx[1][2] = 2;
	Gx[2][0] = -1; Gx[2][1] = 0; Gx[2][2] = 1;

	Gy[0][0] =  1; Gy[0][1] =  2; Gy[0][2] =  1;
	Gy[1][0] =  0; Gy[1][1] =  0; Gy[1][2] =  0;
	Gy[2][0] = -1; Gy[2][1] = -2; Gy[2][2] = -1;

	int  sumX, sumY;

	for(y=0 ; y<m_srcImage.GetYSize() ; y++)
	{
		for(int x=0 ; x<m_srcImage.GetXSize() ; x++)
		{
			sum = sumX = sumY = 0;

			//image boundary
			if(x==0 || x==m_srcImage.GetXSize()-1)
			{
				sum = 0;
				theta = 0;
			}
			else if(y==0 || y==m_srcImage.GetYSize()-1)
			{
				sum = 0;
				theta = 0;
			}
			
			else
			{
				//find gradient
				for(int i=-1 ; i<=1 ; i++)
				{
					for(int j=-1 ; j<=1 ; j++)
					{
						sumX = sumX + (ppTmpData[y+j][x+i] * Gx[i+1][j+1]);
					}
				}

				for(i=-1 ; i<=1 ; i++)
				{
					for(int j=-1 ; j<=1 ; j++)
					{
						sumY = sumY + (ppTmpData[y+j][x+i] * Gy[i+1][j+1]);
					}
				}

				//find the edge strength
				sum = abs(sumX) + abs(sumY);

				//find the edge direction
				if(sumX == 0)
				{
					if(sumY == 0)
					{
						theta = 0;
					}
					else
					{
						theta = 90;
					}
				}
				else
				{
					theta = atan2((float)sumY, (float)sumX)*180.0/3.141593;
				}
	
				//quantize the edge direction
				if((theta > -22.5 && theta < 22.5) || theta > 157.5 || theta < -157.5)
				{
					theta = 0;
				}
				else if((theta >= 22.5 && theta < 67.5) || (theta >= -157.5 && theta < -112.5))
				{
					theta = 45;
				}
				else if((theta >= 67.5 && theta <= 112.5) || (theta >= -112.5 && theta <= -67.5))
				{
					theta = 90;
				}
				else
				{
					theta = 135;
				}
			}
			
			ppStrengthData[y][x] = (int)sum;
			ppNormalData[y][x] = (int)theta;
		}
	}

	/*
	 * non-maximum suppression
	 * ppTmpData <= non-maximum suppressed strength
	 */
	for(y=0 ; y<m_srcImage.GetYSize() ; y++)
	{
		for(int x=0 ; x<m_srcImage.GetXSize() ; x++)
		{
			//image boundary
			if(x==0 || x==m_srcImage.GetXSize()-1)
			{
				ppTmpData[y][x] = 0;
			}
			else if(y==0 || y==m_srcImage.GetYSize()-1)
			{
				ppTmpData[y][x] = 0;
			}

			else
			{
				switch(ppNormalData[y][x])
				{
				case 90:
					if(ppStrengthData[y][x] < ppStrengthData[y][x-1] || ppStrengthData[y][x] < ppStrengthData[y][x+1])
					{
						ppTmpData[y][x] = 0;
					}
					else
					{
						ppTmpData[y][x] = ppStrengthData[y][x];
					}
					break;
				case 135:
					if(ppStrengthData[y][x] < ppStrengthData[y-1][x+1] || ppStrengthData[y][x] < ppStrengthData[y+1][x-1])
					{
						ppTmpData[y][x] = 0;
					}
					else
					{
						ppTmpData[y][x] = ppStrengthData[y][x];
					}
					break;
				case 0:
					if(ppStrengthData[y][x] < ppStrengthData[y-1][x] || ppStrengthData[y][x] < ppStrengthData[y+1][x])
					{
						ppTmpData[y][x] = 0;
					}
					else
					{
						ppTmpData[y][x] = ppStrengthData[y][x];
					}
					break;
				case 45:
					if(ppStrengthData[y][x] < ppStrengthData[y-1][x-1] || ppStrengthData[y][x] < ppStrengthData[y+1][x+1])
					{
						ppTmpData[y][x] = 0;
					}
					else
					{
						ppTmpData[y][x] = ppStrengthData[y][x];
					}
					break;
				}

			}
		}
	}
	

	/*
	 * hysteresis threshold
	 * ppStrengthData <= mark higher strength over high threshold -1, others 0
	 */
	
	//high threshold
	for(y=0 ; y<m_srcImage.GetYSize() ; y++)
	{
		for(int x=0 ; x<m_srcImage.GetXSize() ; x++)
		{
			if(ppTmpData[y][x] > m_nHighThreshold)
			{
				ppStrengthData[y][x] = -1;
			}
			else
			{
				ppStrengthData[y][x] = 0;
			}
		}
	}

	//low threshold
	for(y=0 ; y<m_srcImage.GetYSize() ; y++)
	{
		for(int x=0 ; x<m_srcImage.GetXSize() ; x++)
		{
			if(ppStrengthData[y][x] == -1)
			{
				threshold(x, y);
			}
		}
	}

	ImShow(m_edgeImage, "result");
}


void CExEdgeDlg::threshold(int x, int y)
{
	int** ppStrengthData = (int**)m_strengthImage.Get2DBuffer();
	int** ppTmpData = (int**)m_tmpImage.Get2DBuffer();
	int** ppNormalData = (int**)m_normalImage.Get2DBuffer();
	unsigned char** ppEdgeData = (unsigned char**)m_edgeImage.Get2DBuffer();

	if(ppTmpData[y][x] > m_nLowThreshold && ppStrengthData[y][x] != -2)
	{
		ppEdgeData[y][x] = 0; //set as edge pixel
		ppStrengthData[y][x] = -2; //mark visited
		switch(ppNormalData[y][x])
		{
		case 90:
			threshold(x, y-1);
			threshold(x, y+1);
			break;
		case 135:
			threshold(x-1, y-1);
			threshold(x+1, y+1);
			break;
		case 0:
			threshold(x-1, y);
			threshold(x+1, y);
			break;
		case 45:
			threshold(x+1, y-1);
			threshold(x-1, y+1);
			break;
		}
	}
	
	return;	
}
