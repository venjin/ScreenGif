#include "stdafx.h"
#include "GlobalData.h"
#include "Mouse.h"

#include <gdiplus.h>  
#include <stdio.h>  
using namespace Gdiplus;
#pragma comment(lib,"gdiplus")  

CGlobalData::CGlobalData()
{
	m_rc.left = 0;
	m_rc.top = 0;
	m_rc.right = 0;
	m_rc.bottom = 0;
}


CGlobalData::~CGlobalData()
{
}
CString CGlobalData::GetCurrentFilePath()
{
	CString strFilePath;
	char exeFullPath[MAX_PATH];
	GetModuleFileName(NULL, (LPWCH)exeFullPath, MAX_PATH);
	strFilePath.Format(_T("%s"), exeFullPath);
	return strFilePath;
}
int CGlobalData::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders  
	UINT  size = 0;         // size of the image encoder array in bytes  

	ImageCodecInfo* pImageCodecInfo = NULL;

	//2.��ȡGDI+֧�ֵ�ͼ���ʽ�������������Լ�ImageCodecInfo����Ĵ�Ŵ�С  
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure  

	//3.ΪImageCodecInfo����������ռ�  
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure  

	//4.��ȡ���е�ͼ���������Ϣ  
	GetImageEncoders(num, size, pImageCodecInfo);

	//5.���ҷ��ϵ�ͼ���������Clsid  
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success  
		}
	}

	//6.�ͷŲ���3������ڴ�  
	free(pImageCodecInfo);
	return -1;  // Failure  
}

bool CGlobalData::GetGifPic(CString strBmpPathName, CString strGifPathName)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	//1.��ʼ��GDI+���Ա������GDI+�������Գɹ�����  
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CLSID   encoderClsid;
	Status  stat;

	//7.����Image���󲢼���ͼƬ  
	Image*   image = new Image(strBmpPathName);

	// Get the CLSID of the gif encoder.  
	GetEncoderClsid(L"image/gif", &encoderClsid);

	//8.����Image.Save��������ͼƬ��ʽת�������Ѳ���3)�õ���ͼ�������Clsid���ݸ���  
	stat = image->Save(strGifPathName, &encoderClsid, NULL);

	if (stat == Ok)
	//9.�ͷ�Image����  
	delete image;
	//10.��������GDI+��Դ  
	GdiplusShutdown(gdiplusToken);
	if (stat == Ok)
	{
		return true;

	}
	else
	{
		return false;
	}
}