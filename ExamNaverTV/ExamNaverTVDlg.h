
// ExamNaverTVDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// CExamNaverTVDlg 대화 상자
class CExamNaverTVDlg : public CDialogEx
{
private:

	HINTERNET mh_session;
	HINTERNET mh_connect;
// 생성입니다.
public:
	CExamNaverTVDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXAMNAVERTV_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
