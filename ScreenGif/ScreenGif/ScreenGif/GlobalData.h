#pragma once
class CGlobalData
{
public:
	CGlobalData();
	~CGlobalData();
public:
	RECT m_rc;	//��Ļ��ͼ����
	RECT m_AllScreen; //ȫ���ߴ�
public:
	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	static bool GetGifPic(CString strBmpPathName, CString strGifPathName);
	static CString GetCurrentFilePath();
};

