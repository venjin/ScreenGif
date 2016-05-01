// WellcomPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ScreenGif.h"
#include "WellcomPage.h"
#include "afxdialogex.h"


// CWellcomPage �Ի���

IMPLEMENT_DYNAMIC(CWellcomPage, CDialog)

CWellcomPage::CWellcomPage(CWnd* pParent /*=NULL*/)
	: CDialog(CWellcomPage::IDD, pParent)
{

}

CWellcomPage::~CWellcomPage()
{
}

void CWellcomPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWellcomPage, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWellcomPage ��Ϣ�������


void CWellcomPage::OnPaint()
{
	CPaintDC dc(this); // device context for paintin
	//CDialog::OnPaint();//Ҫ��ֹ�������   
	
	CRect   rect;
	GetClientRect(&rect);
	CDC   dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap   bmpBackground;
	bmpBackground.LoadBitmap(IDB_BITMAP4);   //IDB_BITMAP�����Լ���ͼ��Ӧ��ID   �������Ҹոռ����λͼ��Դ 
	//������������IDB_Bg�������������bmpBackground.LoadBitmap(IDB_Bg);  

	BITMAP   bitmap;
	bmpBackground.GetBitmap(&bitmap);
	CBitmap   *pbmpOld = dcMem.SelectObject(&bmpBackground);
	dc.StretchBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0,
	bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

	// TODO:  �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
}


BOOL CWellcomPage::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowPos(&wndTopMost, 0, 0, 600, 200, SWP_SHOWWINDOW);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetTimer(2, 5000, NULL);
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(CRect(x / 2 - 300, y / 2 - 100, x / 2 + 300, y / 2 + 100));
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CWellcomPage::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == 2)
	{
		
		KillTimer(2);
		DestroyWindow();
	}
	CDialog::OnTimer(nIDEvent);
}
