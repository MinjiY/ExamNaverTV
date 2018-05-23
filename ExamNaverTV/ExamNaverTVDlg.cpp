
// ExamNaverTVDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "ExamNaverTV.h"
#include "ExamNaverTVDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




CExamNaverTVDlg::CExamNaverTVDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EXAMNAVERTV_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExamNaverTVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_cast_list);
}

BEGIN_MESSAGE_MAP(CExamNaverTVDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_LBN_DBLCLK(IDC_LIST1, &CExamNaverTVDlg::OnDblclkCastList)
END_MESSAGE_MAP()


// CExamNaverTVDlg �޽��� ó����

BOOL CExamNaverTVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);	
	/* ��ǻ�Ϳ����� session�� ������ ����, ����̶�� �� �� �ִ�.
	������ �̿��ϸ� �������� ��ϵ� ������ ������ �� �ִ�. */

	mh_session = ::InternetOpen(L"visual_Example", PRE_CONFIG_INTERNET_ACCESS, NULL, INTERNET_INVALID_PORT_NUMBER, 0);
	// ������ �ϳ� ����, ������ �ݱ� ������ ���ͳ� ���õ� �͵��� �� ���� ������ ������ �� ����
	// �ش� �������� ������ �ٸ� �������� �ű���� ����
	mh_connect = ::InternetConnect(mh_session, L"tv.naver.com", INTERNET_INVALID_PORT_NUMBER, L"", L"", INTERNET_SERVICE_HTTP, 0, 0);
	/* 
	   ������ �̿��� ���ӽõ� -> mh_connect������ ����� ����
	   ������ �ּҴ� url�� page�� ������ �����־�� �Ѵ�.
	   Url : www.naver.com
	   Page : /���ĺκ� .htm
	*/
	char error_count = 0;
	// ������ ������ ��û�Ѵ�.
	HINTERNET h_http_file = ::HttpOpenRequest(mh_connect, L"GET", L"/c/entertainment", HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	
	/*
	   Get���� �ش� ������ �䱸�Ѵٰ� ��û
	   �״��� page�̸� �־���� .html
	*/
	
	BOOL request_flag = ::HttpSendRequest(h_http_file, NULL, 0, 0, 0);
	// �б��û -> ���� �ٿ�ε� ����

	if (request_flag == TRUE) {
		CString str, str1;
		char *p_html_data = new char[10*1024*1024];
		/* 
			char *p_html_data�� ���� �Ҵ��� ����
			���� ������� 1�ް��̹Ƿ� �迭�� 10�ް� ������ �����ϸ� ������ (������ �ƴ����� �ȵ��ư�)
			�����Ҵ� �ʿ�-> �����Ҵ��� 2�Ⱑ���� ��밡��!

		*/
		char buffer[1025];
		DWORD read_byte = 0, buffer_size = 1024, total_size = 0;
		// �ִ� 1024 ũ�⸸ŭ �������� ������ �д´�.
		while (::InternetReadFile(h_http_file, buffer, buffer_size, &read_byte)) {
		// ���� �о�ö����� 1024byte�� �д´�.

			buffer[read_byte] = 0;	//���� ���� (���ڿ�)�� ���� NULL(EOL)�� ����Ѵ�. ����� ���� ���� string�� ��ĥ �� ����!


			if (read_byte > 0 && read_byte < 1025) {
				memcpy(p_html_data + total_size, buffer, read_byte);	//�����Ҵ� �� ���� ��Ӵ��س���
				total_size += read_byte;	
				/* total size: �����Ҵ翡 �ű� size
				   read_byte: ���� �о�� ������
				   */

			}	
			if (read_byte < 1024) {
				//1024 ���� ���� ��츦 ���� ���, ���� ���ؼ� ���д� ��쵵 �ֱ� ������ �ִ� 5������ ��õ��Ѵ�.
				// ��õ��� �ϴ� ��� 50ms�� �����ð��� �ΰ��Ͽ� ������ ��ٸ����� �Ѵ�.
				Sleep(50);
				error_count++;
				if (error_count > 5) break;
				// 5ȸ�̻� �������� ����
			}
			else error_count = 0;
		}

		p_html_data[total_size] = 0;  // Null
		//str = p_html_data;
		Convert_UTF8_to_Unicode(p_html_data, &str);


		str.Replace(L"\n", L"\r\n");  // Replace��ɾ�� \n�� \r\n���� �ٲ�
									  // �׷��� Edit Control Box�ȿ��� ������ �����ϴ�.
		
		SetDlgItemText(IDC_HTML_EDIT, str);
		//�ش� ������ ������ ������� ����� �ڵ��� �ݴ´�.
		::InternetCloseHandle(h_http_file);

		char *p_link_url, *p_pos;

		char temp_name[1024];
		char *p_data = p_html_data;
		while (p_data != NULL) {
			//p_data = strstr(p_data, "cds _MM_CARD");
			p_data = strstr(p_data, "cds_info");
			/* 
				strstr: ���ڿ� �ӿ��� ���ڿ��� ã��
				�о�� �������� ��ü���� cds _MM_CARD�� ã�´�.(�������� �ִ� ��ġ���� cds _MM_CARD�� ������)
				ã���� null�� �ƴ� �� ���� �Ǿ� ��ġ�� �ּҸ� ����Ų��. ��ã���� Null��ȯ
			*/
			if (p_data == NULL) break;
			p_data += 8;					//cds MM_CARD ã�� ���� +12���־� �� ���� ��ġ�� �̵�



			p_data = strstr(p_data, "href=");
			p_data += 6;					//�� ���� href= �� ã�Ƽ� ã�� ��ġ���� +6 ���־� �� ���� ��ġ�� �̵�


			p_link_url = new char[1024];	//p_link_url �����Ҵ���
			strcpy_s(p_link_url, 1024, "https://tv.naver.com"); // 23

			/*
				url(tv.naver.com)�� �����Ҵ��� ���� p_link_url�� ����
			*/
//			p_pos = p_link_url +23;

			p_pos = p_link_url;
			/*
				p_pos�� Ƽ����̹����� ������ ��ġ+23�� ������ " �� ���ö� ���� ���� ������
				 ("���� " ���� tvcast.naver.com ���Ŀ� �ٿ����� -> /xxxxx �̷����� �ּҰ� �������Ƿ�..)
			*/
			while (*p_data != '"') {
				*p_pos++ = *p_data++;
			}
			*p_pos = 0;			//������ ���� null��

			char *p_sub_data = strstr(p_data, "<tooltip>");
			// �� �ȿ��� �ٽ��ѹ� ã�Ƽ� <tooltip>�� ã�� (������ ���� ���� �ܾ�)


			if (p_sub_data != NULL) {
				p_sub_data += 9;		// <tooltip>�� ã�� ���� ����Ű�� '<'��ġ���� +9�ϸ� '>'��ġ �������� �̵���
			
				char *p_end_sub_data = strstr(p_sub_data, "</tooltip>");	//	</tooltip>�� ã�´�
				if (p_end_sub_data != NULL) {
					int string_size = p_end_sub_data - p_sub_data;			// </tooltip>�� ã�� ��ġ���� +9�ߴ� p_sub_data�� ��ġ�� ���� ������ ���̰� ���´�. = string_size
					memcpy(temp_name, p_sub_data, string_size);				// ������ 1024ũ��� �Ҵ��ߴ� temp_name�� p_sub_data���� ������̸�ŭ �о�ͼ� ����
					temp_name[string_size] = 0;								// ���������� null
					Convert_UTF8_to_Unicode(temp_name, &str1);				// web�� UTF8���, �츮�� �����ڵ�� ���� ������ �ȸ¾Ƽ� �ѱ��� ��������µ� ���� ��ȯ�ʿ�
					int index = m_cast_list.InsertString(-1, str1);			// �� ������ �ݺ��ؼ� ���� ��ϵ� ������ ���� ����Ʈ�� �����.
					m_cast_list.SetItemDataPtr(index, p_link_url);			// index�� SetItemDataPtr �� URl������ �߰�����
					//����Ʈ�� ���� ���뿡 +4����Ʈ�� ������ ���������Ƿ� (������ ũ��) �װ��� URL�� �����Ŵ
				}

			}

		}
		delete[] p_html_data; 
	}

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CExamNaverTVDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CExamNaverTVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CExamNaverTVDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	int count = m_cast_list.GetCount();
	char *p_data;
	for (int i = 0; i < count; i++) {
		p_data = (char*)m_cast_list.GetItemDataPtr(i);
		delete[] p_data;
	}// ���α׷� ������ ListBox�� index������ŭ �ݺ��ؼ� DataPtr �о�� �� ���δ� �����Ҵ�����

	::InternetCloseHandle(mh_connect);
	::InternetCloseHandle(mh_session);
	//����� �� session�� connect�����Ŵ
}




void CExamNaverTVDlg::OnDblclkCastList()
{// Ŭ���� �����翡�� ��� - ����Ʈ - ����Ŭ�� �޽��� �߻�
	int index = m_cast_list.GetCurSel();
	// Ŭ���� �Ķ������� Ŀ�� üũ
	char *p_data = (char *)m_cast_list.GetItemDataPtr(index);
	//�ش� index�� ������ Ptr�� �о�� ( �ش� ������ �ּ� )
	//void*�� ��ȯ�ϹǷ� char*�� ��ȯ
	//ShellExecuteA(NULL, "Open", p_data, NULL, NULL, SW_SHOW);
	ShellExecuteA(NULL, "Open", "iexplore.exe", p_data, NULL, SW_SHOW);
	//�ش� url����
	//ExcuteA �Լ� ���� �ּ�ó��ó�� �����ϸ� ����Ʈ ���������� �����
	// (�̹� ���ͳ� ����Ǿ��ִ� ��� �� �����쿡 ������ �߰���)
	// iexplore.exe �Ἥ null���� url�ָ� ���� ������Ѽ� ����
}

/* �����ڵ� ��ȯ�Լ� */
/* ������ �� ��Ƽ����Ʈ�� �������µ� �����ڵ�� �ٲٷ��� ��ȯ�� �ؾ��� */
void Convert_UTF8_to_Unicode(char *ap_src_string, CString *ap_dest_string)
{
	// UTF8 �ڵ�� �� ���ڿ��� �����ڵ� ������ ���ڿ��� �����ϱ� ���� �ʿ��� ũ�⸦ ��´�.
	int temp_length = MultiByteToWideChar(CP_UTF8, 0, ap_src_string, -1, NULL, 0);

	if (temp_length > 0) {
		// ��ȯ�� �����ڵ带 ������ ������ �Ҵ��Ѵ�.
		BSTR unicode_str = SysAllocStringLen(NULL, temp_length);
		// �����ڵ�� ��ȯ�Ѵ�.
		MultiByteToWideChar(CP_UTF8, 0, ap_src_string, -1, unicode_str, temp_length);
		*ap_dest_string = unicode_str;
		// �����ڵ� ������ ���ڿ��� �����ϱ� ���� �����ߴ� �޸𸮸� �����Ѵ�.
		SysFreeString(unicode_str);
	}
	else ap_dest_string->Empty();
}
