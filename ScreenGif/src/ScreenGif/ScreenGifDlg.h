
// ScreenGifDlg.h : ͷ�ļ�
//

#pragma once
#include "GlobalData.h"
// CScreenGifDlg �Ի���
#include "RegionDlg.h"
#include "AllScreenDlg.h"
#include "afxcmn.h"
class CWellcomPage;
class CScreenGifDlg : public CDialogEx
{
// ����
public:
	CScreenGifDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CScreenGifDlg();

// �Ի�������
	enum { IDD = IDD_SCREENGIF_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedGetarea();
	afx_msg void OnBnClickedFullscreen();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedShare();
	afx_msg void OnBnClickedGetPic();
	afx_msg void OnBnClickedSave();
	afx_msg BOOL OnToolTipText(UINT NID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg long OnHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg void OnHelp();


public:
	POINT m_Begin;	//ѡ����������Ͻ�����
	POINT m_End;	//ѡ����������½�����
	HHOOK m_myHook;	//��깳�Ӿ��
	HHOOK m_hkey;	// ���̹��Ӿ��

public:
	// �������
	afx_msg LRESULT OnBeginPoint(WPARAM wParam, LPARAM lParam);
	// ����ƶ�ʱ������
	//afx_msg LRESULT OmMouseMove(WPARAM wParam, LPARAM lParam);
	// �����յ�
	afx_msg LRESULT OnEndPoint(WPARAM wParam, LPARAM lParam);
	// ��ͼ��ݼ���Ϣ
	afx_msg LRESULT OnScreenPic(WPARAM wParam, LPARAM lParam);
	// �߳�֪ͨ��Ϣ
	afx_msg LRESULT OnProgress(WPARAM wParam, LPARAM lParam);
	// �߳�֪ͨ����
	afx_msg LRESULT OnThreadEnd(WPARAM wParam, LPARAM lParam);

public:
	// ��ͼ
	bool GetPic();
	// ����bitmapͼƬ
	void SaveBitmapToFile(CBitmap* pBitmap, CString fileName);
	// ����gifͼƬ
	void MakeGif();
	// ��ӹ�����
	void AddToolbar();
	static DWORD WINAPI ThreadMakeGif(LPVOID lpParam);



public:
	CGlobalData m_Data;				//������
	CRegionDlg* m_pRegionDlg;		//ѡȡ����
	CAllScreenDlg* m_pAllScreenDlg; //��Ļ�Ի���
	BOOL m_bStart;					//����ѡʱ�ı�־
	CDC m_srcDc;					//��ĻDC
	CDC m_dcCompatible;				//����DC
	CBitmap m_mapCompatible;		//����λͼ
	int m_index;				//bmpͼ���
	bool m_bIsReadyScreen;			//�Ƿ���Խ�ͼ���Ƿ�ѡ���˽�ͼ����
	bool m_bIsReadyGif;				//�Ƿ����¼��Gif
	bool m_bFirstGif;				//��һ��Gif
	CFile* m_pGifFile;				//Gif�ļ�
	WORD m_wGifbeginSize;			//Gif�ļ���ʼ��ͼ�ο�����չ��Ĵ�С
	WORD m_wGifDataSize;			//Gif�ļ�ͼ�����ݲ��ֵĴ�С
	bool m_bMouseHook;				//��깳���Ƿ��Ѿ��趨
	CString m_strPath;				//pic�ļ�·��
	CString m_strCurentGif;			//��ǰ������gif
	bool m_bIsPicexistInDc;			//�ڴ����Ƿ��Ѿ�����ͼƬ
	CBrush m_brush;					//�Ի��򱳾���ɫ
	CToolBar m_toolbar;				//������
	CImageList m_toolbarlist;		//ͼ������
	CToolBar m_toolbarchange;		//�������л�
	CImageList m_toolbarlistchange;	//�л�ͼ������
	NOTIFYICONDATA m_notify;
	CWellcomPage* m_wellcom;		//��������

	CToolTipCtrl m_tooltip;			//������tooltip
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNotifyMsg(WPARAM wparam, LPARAM lparam);
	virtual void OnCancel();
	afx_msg void OnBnClickedCancel();
	// ������
	CProgressCtrl m_Progress;
};
