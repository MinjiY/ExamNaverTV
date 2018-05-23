
// ExamNaverTVDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"


// CExamNaverTVDlg ��ȭ ����
class CExamNaverTVDlg : public CDialogEx
{
private:

	HINTERNET mh_session;
	HINTERNET mh_connect;
// �����Դϴ�.
public:
	CExamNaverTVDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXAMNAVERTV_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
protected:
	CListBox m_cast_list;
public:
	afx_msg void OnDblclkCastList();
};

void Convert_UTF8_to_Unicode(char * ap_src_string, CString * ap_dest_string);
