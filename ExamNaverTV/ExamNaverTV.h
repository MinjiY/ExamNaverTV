
// ExamNaverTV.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CExamNaverTVApp:
// �� Ŭ������ ������ ���ؼ��� ExamNaverTV.cpp�� �����Ͻʽÿ�.
//

class CExamNaverTVApp : public CWinApp
{
public:
	CExamNaverTVApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CExamNaverTVApp theApp;