#pragma once


// CRegionDlg �Ի���

class CRegionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRegionDlg)

public:
	CRegionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRegionDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_REGION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	RECT m_rect;	//�Ի���Ĵ�С
	virtual void PostNcDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
private:
	CBrush m_brush;
};
