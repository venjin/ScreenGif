#pragma once


// CWellcomPage �Ի���

class CWellcomPage : public CDialog
{
	DECLARE_DYNAMIC(CWellcomPage)

public:
	CWellcomPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWellcomPage();

// �Ի�������
	enum { IDD = IDD_DIALOG_WELLCOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
