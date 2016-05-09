
// ScreenGifDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ScreenGif.h"
#include "ScreenGifDlg.h"
#include "afxdialogex.h"
#include "WellcomPage.h"
#include "SgifAbout.h"
#include "PicProcess.h"
#include "PicMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScreenGifDlg �Ի���




CScreenGifDlg::CScreenGifDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScreenGifDlg::IDD, pParent)
{
	m_index = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pRegionDlg = NULL;
	//�����ʾ����Ļ�ߴ�
	m_AllScreen.left = 0;
	m_AllScreen.top = 0;
	m_AllScreen.right = GetSystemMetrics(SM_CXSCREEN);
	m_AllScreen.bottom = GetSystemMetrics(SM_CYSCREEN);
	// ������ĻDC
	m_srcDc.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	m_bIsReadyGif = false;
	m_bFirstGif = false;
	m_pGifFile = NULL;
	m_wGifbeginSize = 0;
	m_strCurentGif = _T("");
	m_brush.CreateSolidBrush(RGB(37, 37, 38));
	//�ڳ���Ŀ¼�´���picĿ¼
	m_strPath = CGlobalData::GetCurrentFilePath();
	m_strPath = m_strPath.Left(m_strPath.ReverseFind('\\'));
	m_strPath += _T("\\pic");
	if (!PathFileExists(m_strPath))
	{
		CreateDirectory(m_strPath, NULL);
	}
	//��ʼ������
	m_pen.m_style = PS_SOLID;
	m_pen.m_width = 2;
	m_pen.m_color = RGB(255, 0, 0);
	m_rcbrush.CreateSolidBrush(NULL_BRUSH);
}


CScreenGifDlg::~CScreenGifDlg()
{
	NOTIFYICONDATA nd;
	nd.cbSize = sizeof (NOTIFYICONDATA);
	nd.hWnd = m_hWnd;
	nd.uID = NIF_ICON;
	Shell_NotifyIcon(NIM_DELETE, &nd);
	UnregisterHotKey(m_hWnd, HOTKEYID);
	if (m_wellcom != NULL)
	{
		delete m_wellcom;
		m_wellcom = NULL;
	}
	if (m_process != NULL)
	{
		delete m_process;
	}
}
void CScreenGifDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
}

BEGIN_MESSAGE_MAP(CScreenGifDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_GETAREA, &CScreenGifDlg::OnBnRectangle)
	ON_BN_CLICKED(ID_START, &CScreenGifDlg::OnBnClickedStart)
	ON_BN_CLICKED(ID_SHARE, &CScreenGifDlg::OnBnClickedShare)
	ON_BN_CLICKED(ID_GETPIC, &CScreenGifDlg::OnBnClickedGetPic)
	ON_BN_CLICKED(ID_SAVE, &CScreenGifDlg::OnBnClickedSave)
	ON_MESSAGE(UM_BEGINPOINT, OnBeginPoint)
	ON_MESSAGE(UM_ENDPOINT, OnEndPoint)
	ON_MESSAGE(UK_REGION, OnScreenPic)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
	ON_MESSAGE(WM_USER_NOTIFYICON, OnNotifyMsg)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_COMMAND(WM_USEHELP,	OnHelp)
	ON_COMMAND(WM_ABOUT, OnAbout)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CANCEL, &CScreenGifDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CScreenGifDlg ��Ϣ�������
HRESULT CScreenGifDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	if (wParam == HOTKEYID)
	{
		GetRect();
	}
	return TRUE;
}


// ��ȡ�յ�����
LRESULT CScreenGifDlg::OnEndPoint(WPARAM wParam, LPARAM lParam)
{
	POINT* pt = (POINT*)lParam;
	m_rc.right = pt->x;
	m_rc.bottom = pt->y;
	bool isAllScreen = false;
	if (pt->x -  m_rc.left == 0 || pt->y - m_rc.top == 0)//���������Чȡȫ��
	{
		//��ȡ����ȫ���ߴ�
		m_rc = m_AllScreen;
		isAllScreen = true;
	}
	//��������ʾ��ѡ�Ի���
	if (m_pRegionDlg == NULL)
	{
		m_pRegionDlg = new CRegionDlg;
		m_pRegionDlg->m_rect = m_rc;
		m_pRegionDlg->Create(IDD_DIALOG_REGION, NULL);
		
		m_pRegionDlg->ShowWindow(SW_SHOW);
		Invalidate();
	}
	RECT rect;
	GetWindowRect(&rect);
	RECT rt;
	rt.left = m_rc.right - (rect.right - rect.left);
	rt.top = m_rc.bottom;
	rt.right = m_rc.right;
	rt.bottom = m_rc.bottom + (rect.bottom - rect.top);
	if (isAllScreen)
	{
		rt.top = 0;
		rt.right = m_rc.right;
		rt.bottom = rect.bottom - rect.top;
	}
	SetWindowPos(&wndTopMost, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, SWP_SHOWWINDOW);
	MoveWindow(&rt);
	ShowWindow(SW_SHOW);
	//���ر����Ի���
	if (m_pAllScreenDlg->IsWindowVisible())
	{
		m_pAllScreenDlg->ShowWindow(SW_HIDE);
	}
	
	return TRUE;
}

// ��ȡ��ʼ������
LRESULT CScreenGifDlg::OnBeginPoint(WPARAM wParam, LPARAM lParam)
{
	POINT* pt = (POINT*)lParam;
	m_rc.left = pt->x;
	m_rc.top = pt->y;

	return TRUE;
}

// ��ͼ��ݼ���Ϣ
LRESULT CScreenGifDlg::OnScreenPic(WPARAM wParam, LPARAM lParam)
{
	//ָ������
	GetRect();
	return TRUE;
}
// ��ӹ�����
void CScreenGifDlg::AddToolbar()
{
	GetDlgItem(ID_GETAREA)->ShowWindow(SW_HIDE);
	GetDlgItem(ID_SAVE)->ShowWindow(SW_HIDE);
	GetDlgItem(ID_START)->ShowWindow(SW_HIDE);
	GetDlgItem(ID_GETPIC)->ShowWindow(SW_HIDE);
	GetDlgItem(ID_SHARE)->ShowWindow(SW_HIDE);
	GetDlgItem(ID_FULLSCREEN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CANCEL)->ShowWindow(SW_HIDE);

	if (m_toolbar.CreateEx(this, CBRS_FLYBY | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_TOP | CBRS_TOOLTIPS))
	{
		
		static UINT BASED_CODE DockTool[] = { ID_GETAREA, ID_SAVE, ID_START, IDC_CANCEL, ID_GETPIC  , ID_SHARE};
		CBitmap bitmap;
		bitmap.LoadBitmapW(IDB_BITMAP1);
		m_toolbarlist.Create(16, 16, ILC_COLOR24, 0, 0);
		m_toolbarlist.Add(&bitmap, (CBitmap*)NULL);
	
		//���ù�������ťͼƬ
		m_toolbar.GetToolBarCtrl().SetImageList(&m_toolbarlist);
		//���ù�������ť��С�� �Ͱ�ť��λͼ��С
		SIZE sbutton, sImage;
		sbutton.cx = 33;
		sbutton.cy = 32;
		sImage.cx = 16;
		sImage.cy = 16;
		m_toolbar.SetSizes(sbutton, sImage);
		m_toolbar.SetButtons(DockTool, (UINT)6);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	}
	

}
BOOL CScreenGifDlg::OnToolTipText(UINT NID, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	//  UNICODE��Ϣ
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	//TCHAR   szFullText[512];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;
	UINT tempid;
	UINT fid;
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags&TTF_IDISHWND) || pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags&TTF_IDISHWND))
	{
		//idFromΪ��������HWND
		nID = ::GetDlgCtrlID((HWND)nID);
		//tem = m_toolbar.CommandToIndex(pNMHDR->idFrom);
		  
		  HWND hWnd = ::GetFocus();
		  tempid = m_toolbar.GetToolBarCtrl().GetDlgCtrlID();// ::GetDlgCtrlID(hWnd);
		 
	}
	if (nID != 0)   //��Ϊ�ָ���
	{
		tempid = m_toolbar.GetToolBarCtrl().GetHotItem();
		fid = m_toolbar.GetItemID(tempid);
		strTipText.LoadString(fid);
		
		strTipText = strTipText.Mid(strTipText.Find(_T("/n"), 0) + 1);
#ifndef   _UNICODE
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else
		{
			_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#else
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			_wcstombsz(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else
		{
			lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#endif
		*pResult = 0;
		// ʹ��������ʾ������������
		CRect rcChild;
		GetWindowRect(&rcChild);
	
		
		//ScreenToClient(rcChild);
		int x = 0, y = rcChild.top;
		if (tempid == 0)
		{
			x = rcChild.left - 50;
			y = rcChild.top + 40;
		}
		else if (tempid == 1)
		{
			x = rcChild.left +30;
			y = rcChild.top + 40;

		}
		else if (tempid == 2)
		{
			x = rcChild.left + 65;
			y = rcChild.top + 40;

		}
		else if (tempid == 3)
		{
			x = rcChild.left +100;
			y = rcChild.top + 40;

		}
		else if (tempid == 4)
		{
			x = rcChild.left +130;
			y = rcChild.top + 40;

		}
		else if (tempid == 5)
		{
			x = rcChild.left +165;
			y = rcChild.top + 40;

		}
		
		::MoveWindow(pNMHDR->hwndFrom, x, y, rcChild.Width(), rcChild.Height(), TRUE);
		return TRUE;
	}
	return TRUE;

}
	

BOOL CScreenGifDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	EnableToolTips(TRUE);
	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	m_wellcom = new CWellcomPage;
	m_wellcom->Create(CWellcomPage::IDD, NULL);
	m_wellcom->ShowWindow(SW_SHOW);
	m_Progress.ShowWindow(SW_HIDE);	//Ӱ�ؽ�����
	SetTimer(3, 200, NULL);
	
	::RegisterHotKey(m_hWnd, HOTKEYID, MOD_CONTROL|MOD_ALT|MOD_WIN, NULL);//ע��ȫ�ֿ�ݼ� CTRL + WIN + ALT
	AddToolbar();	//��ӹ�����
	
	m_notify.cbSize = sizeof NOTIFYICONDATA;
	m_notify.hWnd = this->m_hWnd;
	m_notify.uID = IDR_MAINFRAME;
	m_notify.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	CString str = _T("SGif��ͼ");
	CStringW strWide = CT2CW(str); // ��T�ַ�ת���ɿ��ַ�
	wcscpy_s(m_notify.szTip, strWide);
	m_notify.uCallbackMessage = WM_USER_NOTIFYICON;
	m_notify.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;    
	Shell_NotifyIcon(NIM_ADD, &m_notify);	//������ͼ�����������
	//����
	m_map = new CPicMap;
	m_map->Create(CPicMap::IDD, NULL);
	m_map->ShowWindow(SW_HIDE);
	//��ͼ����
	m_process = new CPicProcess;
	m_process->m_rc = m_rc;
	m_map->m_pProcess = m_process; //��������ǰͼ���ַ
	m_process->m_map = m_map; //���滭����ַ
	m_process->Create(CPicProcess::IDD, NULL);	
	m_process->ShowWindow(SW_HIDE);
	


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CScreenGifDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CScreenGifDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//���ι���
void CScreenGifDlg::OnBnRectangle()
{
	//��ѡ����ι��ߵ�ʱ���ȸ���ͼ���򸲸Ǵ��ڣ�����Ҫ�ڸô��������ͼ
	if (m_process != NULL && m_map != NULL)
	{
		GetPic();	//�Ƚ�ͼ
		m_process->m_bRect = true;
		m_process->m_rc = m_rc;
		m_map->m_curRect = m_rc;
		m_process->m_rect.SetRectEmpty();
		m_process->m_vecRect.clear();
		m_map->SetWindowPos(&wndTop, m_rc.left + 2, m_rc.top + 2,
			m_rc.right - m_rc.left - 4, m_rc.bottom - m_rc.top - 4, SWP_SHOWWINDOW);
		m_process->SetWindowPos(&wndTopMost, m_rc.left + 2, m_rc.top + 2, 
			m_rc.right - m_rc.left - 4, m_rc.bottom - m_rc.top - 4, SWP_SHOWWINDOW);

	}	
}

// ָ������
void CScreenGifDlg::GetRect()
{
	ShowWindow(SW_HIDE);
	if (m_pRegionDlg != NULL)
	{
		m_pRegionDlg->DestroyWindow();
		m_pRegionDlg = NULL;
	}
	if (m_pAllScreenDlg != NULL)	//�����ѡ��ťʱ������֮ǰ�ı����Ի��򣬴����µ�
	{
		m_pAllScreenDlg->DestroyWindow();
		m_pAllScreenDlg = NULL;
	}
	m_pAllScreenDlg = new CAllScreenDlg;		//�����µ�ȫ��Ļ�����Ի���
	m_pAllScreenDlg->m_rc = m_AllScreen;
	m_pAllScreenDlg->Create(IDD_DIALOG_ALLSCREEN, NULL);
	m_pAllScreenDlg->ShowWindow(SW_SHOW);
}

// TODO:  ����Gifͼ
void CScreenGifDlg::OnBnClickedStart()
{
	if (m_process->IsWindowVisible())
	{
		m_process->ShowWindow(SW_HIDE);
	}
	if (m_map->IsWindowVisible())
	{
		m_map->ShowWindow(SW_HIDE);
	}
	if (m_bIsReadyGif)
	{
		//Gif¼�ƽ���
		//�ָ���ť����
		(m_toolbar.GetToolBarCtrl()).EnableButton(ID_GETAREA, true);
		(m_toolbar.GetToolBarCtrl()).EnableButton(ID_SHARE, true);
		(m_toolbar.GetToolBarCtrl()).EnableButton(ID_SAVE, true);
		(m_toolbar.GetToolBarCtrl()).EnableButton(ID_GETPIC, true);
		(m_toolbar.GetToolBarCtrl()).EnableButton(IDC_CANCEL, true);

		m_toolbar.SetButtonInfo(m_toolbar.CommandToIndex(ID_START), ID_START, TBBS_BUTTON, 6);
		
		SetDlgItemText(ID_START, _T("¼��GIF"));
		m_pRegionDlg->DestroyWindow();	//����ѡ��
		m_pRegionDlg = NULL;
		m_bIsReadyGif = false;
		//�رն�ʱ��
		KillTimer(1);
		m_Progress.SetRange(0, m_index);
		m_Progress.SetStep(1);
		m_Progress.ShowWindow(SW_SHOW);
		MakeGif();	//����GifͼƬ
		ShowWindow(SW_HIDE);
		m_pGifFile->Close();
		if (IDYES == ::MessageBox(NULL, _T("GifͼƬ��ɣ�\n�Ƿ����Ϊ"), _T("ScreenGif"), MB_ICONQUESTION|MB_YESNO))
		{
			//���湦��
			CString filter;
			filter = _T("*.gif||");
			CString strTime; //��ȡϵͳʱ�� ����
			CTime tm;
			tm = CTime::GetCurrentTime();
			strTime = tm.Format("%Y%m%d%H%M%S");
			CString strDefName = _T("SGif") + strTime + _T(".gif");
			CFileDialog dlg(FALSE, NULL, LPCTSTR(strDefName), OFN_HIDEREADONLY, filter);
			if (dlg.DoModal() == IDOK)
			{
				CString str;
				str = dlg.GetPathName();
				if (str.Find(_T(".gif")) == -1) // ȷ���û�������ȷ
				{
					str += _T(".gif");
				}
				if (!str.IsEmpty() && !m_strCurentGif.IsEmpty())
				{
					MoveFile(m_strCurentGif, str);
				}
			}
		}
		return;
	}

	//�ı�ͼ��
	m_bIsReadyGif = true;
	CImageList* imagList = (m_toolbar.GetToolBarCtrl()).GetImageList();
	int toolbarCount = imagList->GetImageCount();
	CBitmap cbmp;
	BITMAP bmp;
	cbmp.LoadBitmapW(IDB_BITMAP3);
	cbmp.GetBitmap(&bmp);
	imagList->Add(&cbmp, RGB(192, 192, 192));
	(m_toolbar.GetToolBarCtrl()).SetImageList(imagList);
	imagList->Detach();
	imagList = NULL;
	m_toolbar.SetButtonInfo(m_toolbar.CommandToIndex(ID_START), ID_START, TBBS_BUTTON, 7);
	//�ǰ�ťʧȥ����
	(m_toolbar.GetToolBarCtrl()).EnableButton(ID_GETAREA, false);
	(m_toolbar.GetToolBarCtrl()).EnableButton(ID_SHARE, false);
	(m_toolbar.GetToolBarCtrl()).EnableButton(ID_SAVE, false);
	(m_toolbar.GetToolBarCtrl()).EnableButton(ID_GETPIC, false);
	(m_toolbar.GetToolBarCtrl()).EnableButton(IDC_CANCEL, false);
	SetDlgItemText(ID_START, _T("ֹͣ��"));
	SetTimer(1, 50, NULL);	//���ö�ʱ����ÿ��50ms��һ��ͼ������
	m_index = 0;
	m_wGifbeginSize = 0;
	m_pGifFile = new CFile;
	CString strTime; //��ȡϵͳʱ�� ����
	CTime tm;
	tm = CTime::GetCurrentTime();
	strTime = tm.Format("%Y%m%d%H%M%S");
	CString strDefName = m_strPath + _T("\\SGif") + strTime + _T(".gif");
	m_strCurentGif = strDefName;
	m_pGifFile->Open(strDefName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);

	
}



// TODO:  ������
void CScreenGifDlg::OnBnClickedShare()
{
	// TODO:  ������
	if (m_process->IsWindowVisible())
	{
		m_process->ShowWindow(SW_HIDE);
	}
	if (m_map->IsWindowVisible())
	{
		m_map->ShowWindow(SW_HIDE);
	}
	CString strPath = _T("http://service.weibo.com/share/share.php?appkey=583395093&title=");
	CString strWords = _T("΢������");
	CString strContext = strPath + strWords;
	OnBnClickedCancel();
	ShellExecute(NULL, _T("open"), (LPCTSTR)strContext, _T(""), _T(""), SW_SHOW);

}

// TODO:  ��ͼ
void CScreenGifDlg::OnBnClickedGetPic()
{

	// ��ͼ����

	GetPic();
	if (m_process->IsWindowVisible())
	{
		m_process->ShowWindow(SW_HIDE);
	}
	if (m_map->IsWindowVisible())
	{
		m_map->ShowWindow(SW_HIDE);
	}
	m_mapCompatible.DeleteObject();
	m_pRegionDlg->DestroyWindow();
	m_pRegionDlg = NULL;

	ShowWindow(SW_HIDE);
}

// TODO:  ���Ϊ
void CScreenGifDlg::OnBnClickedSave()
{
	GetPic();
	if (m_process->IsWindowVisible())
	{
		m_process->ShowWindow(SW_HIDE);
	}
	if (m_map->IsWindowVisible())
	{
		m_map->ShowWindow(SW_HIDE);
	}
	ShowWindow(SW_HIDE);
	//m_bIsPicexistInDc = false;
	if (m_pRegionDlg != NULL)
	{
		m_pRegionDlg->DestroyWindow();
		m_pRegionDlg = NULL;
	}
	CString filter;
	filter = _T("*.bmp | *.BMP ||");
	CString strTime; //��ȡϵͳʱ�� ����
	CTime tm; 
	tm = CTime::GetCurrentTime();
	strTime = tm.Format("%Y%m%d%H%M%S");
	CString strDefName = _T("SGif��ͼ") + strTime + _T(".bmp");
	CFileDialog dlg(FALSE, NULL, LPCTSTR(strDefName), OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		CString str;
		str = dlg.GetPathName();
		if (str.Find(_T(".bmp")) == -1 || str.Find(_T(".BMP")) == -1) // ȷ���û�������ȷ
		{
			str += _T(".bmp");
		}
		CBitmap* pbmp = m_dcCompatible.GetCurrentBitmap();
		if (pbmp != NULL)
		{
			SaveBitmapToFile(pbmp, str);
		}
	}
	m_mapCompatible.DeleteObject();
		
}

bool CScreenGifDlg::GetPic()
{
	// ��������DC
	if (m_dcCompatible.m_hDC == NULL)
	{
		m_dcCompatible.CreateCompatibleDC(&m_srcDc);
	}

	if (m_process->m_SrcMemDC.m_hDC == NULL)
	{
		m_process->m_SrcMemDC.CreateCompatibleDC(&m_srcDc);
	}
	
	if (m_map->m_SrcMemDC.m_hDC == NULL)
	{
		m_map->m_SrcMemDC.CreateCompatibleDC(&m_srcDc);
	}


	// ����λͼ����DC
	if (m_mapCompatible.m_hObject == NULL)
	{
		m_mapCompatible.CreateCompatibleBitmap(&m_srcDc, m_rc.right - m_rc.left - 4, m_rc.bottom - m_rc.top - 4);

	}
	CBitmap bmp1, bmp2;
	bmp1.CreateCompatibleBitmap(&m_srcDc, m_rc.right - m_rc.left - 4, m_rc.bottom - m_rc.top - 4);
	bmp2.CreateCompatibleBitmap(&m_srcDc, m_rc.right - m_rc.left - 4, m_rc.bottom - m_rc.top - 4);


	// ������λͼѡ�����DC��
	m_dcCompatible.SelectObject(&m_mapCompatible);
	m_process->m_SrcMemDC.SelectObject(&bmp1);
	m_map->m_SrcMemDC.SelectObject(&bmp2);
	// ��ԭʼ�豸��ɫ���������ݿ鸴�Ƶ�����DC��
	m_dcCompatible.BitBlt(0, 0, m_rc.right - m_rc.left - 4, m_rc.bottom - m_rc.top - 4, &m_srcDc, m_rc.left + 2, m_rc.top + 2, SRCCOPY);
	m_process->m_SrcMemDC.BitBlt(0, 0, m_rc.right - m_rc.left - 4, m_rc.bottom - m_rc.top - 4, &m_srcDc, m_rc.left + 2, m_rc.top + 2, SRCCOPY);
	m_map->m_SrcMemDC.BitBlt(0, 0, m_rc.right - m_rc.left - 4, m_rc.bottom - m_rc.top - 4, &m_srcDc, m_rc.left + 2, m_rc.top + 2, SRCCOPY);
	CClientDC dc(this);
	//RECT displayRect;
	// ��λͼ�����ڼ��а���
	if (OpenClipboard())
	{
		//��ռ��а�
		EmptyClipboard();
		//����λͼ
		SetClipboardData(CF_BITMAP, m_dcCompatible.GetCurrentBitmap()->m_hObject);
		CloseClipboard();
	}
	return true;
	
}

void CScreenGifDlg::MakeGif()
{
	int n = m_index;
	m_index = 0;
	m_bFirstGif = true;
	while (n--)
	{
		CString strbmpName;
		CString strgifName;
		strgifName.Format(_T("\\SGif%d.gif"), m_index);
		strgifName = m_strPath + strgifName;
		m_index++;
		m_Progress.StepIt();
		UpdateData(FALSE);
		m_Progress.UpdateWindow();
		m_Progress.RedrawWindow();
		if (m_bFirstGif)
		{
			CFile gif;
			gif.Open(strgifName, CFile::modeRead | CFile::typeBinary);
			BYTE byte = 0x00;
			while (1)
			{
				gif.Read(&byte, 1);
				m_wGifbeginSize++;
				if (byte == 0x21)
				{
					m_pGifFile->Write(&byte, 1);
					GIFCONTROL gifcontrol;
					gif.Read(&gifcontrol, sizeof(GIFCONTROL));
					gifcontrol.m_wDelayTime = DELAYTIME;
					m_pGifFile->Write(&gifcontrol, sizeof(GIFCONTROL));
					while (gif.Read(&byte, 1) > 0)
					{
						m_pGifFile->Write(&byte, 1);
					}
					break;
				}
				else
				{
					m_pGifFile->Write(&byte, 1);

				}

			}
			//��һ��gif�ļ���ȡ����
			gif.Close();
			::DeleteFile(strgifName);
			m_bFirstGif = false;
		}
		else   //�ǵ�һ��gif�ļ�
		{
			CGif::CombineGif(m_pGifFile, strgifName, m_wGifbeginSize);	//���gifͼƬƴ�ӳ�һ��
			::DeleteFile(strgifName);

		}
	}
	BYTE byte = 0x3B;
	m_pGifFile->Write(&byte, 1);
	m_Progress.ShowWindow(SW_HIDE);
}



//����bitmapͼ
void CScreenGifDlg::SaveBitmapToFile(CBitmap* pBitmap, CString fileName)
{
	//����λͼ�ļ�ÿ��������ռ�ֽ���    
	WORD wBitCount = 0;//ÿ��������ռλ��
	HDC hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	int iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 16)
		wBitCount = 16;
	else if (iBits <= 24)
		wBitCount = 24;
	else if (iBits <= 32)
		wBitCount = 32;
	//�����ɫ���С    
	DWORD dwPaletteSize = 0;
	if (wBitCount <= 16)
	{
		dwPaletteSize = (1 << wBitCount) * sizeof (RGBQUAD);
	}
	//����λͼ��Ϣͷ�ṹ  
	BITMAPINFOHEADER bi; //λͼ��Ϣͷ�ṹ   
	BITMAP Bitmap;	//λͼ���Խṹ
	GetObject(pBitmap->m_hObject, sizeof (BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof (BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	//λͼ�����ֽڴ�С
	DWORD dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
	//Ϊλͼ���ݷ����ڴ�  
	LPBITMAPINFOHEADER lpbi; //ָ��λͼ��Ϣͷ�ṹ   
	HANDLE hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof (BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// �����ɫ��         
	HANDLE hPal = GetStockObject(DEFAULT_PALETTE);
	HANDLE hOldPal = NULL;
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	//   ��ȡ�õ�ɫ�����µ�����ֵ    
	GetDIBits(hDC, (HBITMAP)(pBitmap->m_hObject), 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof (BITMAPINFOHEADER)+dwPaletteSize,
		(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

	//�ָ���ɫ��       
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}    
	BITMAPFILEHEADER bmfHdr; //λͼ�ļ�ͷ�ṹ   
	bmfHdr.bfType = 0x4D42;     //   "BM"    
	DWORD dwDIBSize = sizeof (BITMAPFILEHEADER)
		+sizeof (BITMAPINFOHEADER)
		+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof (BITMAPFILEHEADER)
		+(DWORD)sizeof (BITMAPINFOHEADER)
		+dwPaletteSize;
	//����λͼ�ļ�
	CFile bmpfile;
	bmpfile.Open(fileName, CFile::modeCreate |CFile::modeWrite | CFile::typeBinary);
	//   д��λͼ�ļ�ͷ    
	bmpfile.Write(&bmfHdr, sizeof(BITMAPFILEHEADER));
	//   д��λͼ�ļ���������    
	bmpfile.Write(lpbi,dwDIBSize);
	//���            
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	bmpfile.Close();

}

// ÿ��50ms��ȡһ��ͼ
void CScreenGifDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//�����bmp�ļ�
	if (nIDEvent == 1)
	{
		CString strbmpName;
		CString strgifName;
		strbmpName.Format(_T("\\Bitmap%d.bmp"), m_index);
		strbmpName = m_strPath + strbmpName;
		strgifName.Format(_T("\\SGif%d.gif"), m_index);
		strgifName = m_strPath + strgifName;
		m_index++;
		if (m_bIsReadyGif)	//�Ƿ�����¼��
		{
			GetPic();
			CBitmap* pbmp = m_dcCompatible.GetCurrentBitmap();
			if (pbmp != NULL)
			{
				SaveBitmapToFile(pbmp, strbmpName);
			}
			m_mapCompatible.DeleteObject();
		}
		//��bmpͼƬת����gifͼƬ
		CGif::GetGifPic(strbmpName, strgifName);
		::DeleteFile(strbmpName);
	}
	if (nIDEvent == 3)
	{
		this->ShowWindow(SW_HIDE);
		KillTimer(3);
	}
	CDialogEx::OnTimer(nIDEvent);
}


HBRUSH CScreenGifDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	
		return m_brush;
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}



LRESULT  CScreenGifDlg::OnNotifyMsg(WPARAM wparam, LPARAM lparam)
{
	if (wparam != IDR_MAINFRAME)
		return    1;
	switch (lparam)
	{
		case  WM_RBUTTONUP://�Ҽ�����ʱ������ݲ˵�������ֻ��һ�����رա�     
		{
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint);//�õ����λ��     
			CMenu menu;
		
			menu.CreatePopupMenu();//����һ������ʽ�˵�     
			menu.AppendMenu(MF_STRING, WM_USEHELP, _T("����"));
			menu.AppendMenu(MF_STRING, WM_ABOUT, _T("����"));
			menu.AppendMenu(MF_STRING, WM_DESTROY, _T("�˳�"));
			//ȷ������ʽ�˵���λ��
			SetForegroundWindow();
			menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);
			//��Դ����     
			HMENU hmenu = menu.Detach();
			menu.DestroyMenu();
			delete lpoint;
		}
		break;
		case    WM_LBUTTONDBLCLK://˫������Ĵ���     
		{
			 NULL;//�򵥵���ʾ���������¶�     
		}
		break;
	}
	return 0;
}

//ȡ��
void CScreenGifDlg::OnBnClickedCancel()
{
	if (m_process->IsWindowVisible())
	{
		m_process->ShowWindow(SW_HIDE);
	}
	if (m_map->IsWindowVisible())
	{
		m_map->ShowWindow(SW_HIDE);
	}
	if (m_pRegionDlg != NULL)
	{
		m_pRegionDlg->ShowWindow(SW_HIDE);
	}
	ShowWindow(SW_HIDE);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void CScreenGifDlg::OnHelp()
{
	CString strHelp = _T("���������ʵ�ֽ�ͼ��gifͼƬ�����Ĺ��ܣ������ʹ�÷�ʽΪ��������������ͣ�����������С�");
	strHelp += _T("����Ҫ��ͼ��Ҫ¼��һ��gifͼ��ʱ��ʹ�ÿ�ݼ� CTRL + WIN + ALT ������ѡȡ��Ļ�����ͼ��");
	strHelp += _T("�̰�ť������ɽ�ͼ�����水ť���Ա���ͼƬ����ȡ����ͼ������ʼ¼��gif���ٴΰ��½���gif¼�ơ�");

	::MessageBox(NULL, strHelp, _T("����"), MB_YESNO | MB_ICONINFORMATION);

}

void CScreenGifDlg::OnAbout()
{
	CSgifAbout aboutdlg;
	aboutdlg.DoModal();
}