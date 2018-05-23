
// ExamNaverTVDlg.cpp : 구현 파일
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


// CExamNaverTVDlg 메시지 처리기

BOOL CExamNaverTVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);	
	/* 컴퓨터에서의 session은 일종의 연결, 통신이라고 볼 수 있다.
	세션을 이용하면 브라우저에 등록된 정보를 공유할 수 있다. */

	mh_session = ::InternetOpen(L"visual_Example", PRE_CONFIG_INTERNET_ACCESS, NULL, INTERNET_INVALID_PORT_NUMBER, 0);
	// 세션을 하나 만듬, 세션을 닫기 전까지 인터넷 관련된 것들은 이 세션 정보를 공유할 수 있음
	// 해당 브라우저의 정보를 다른 브라우저로 옮길수도 있음
	mh_connect = ::InternetConnect(mh_session, L"tv.naver.com", INTERNET_INVALID_PORT_NUMBER, L"", L"", INTERNET_SERVICE_HTTP, 0, 0);
	/* 
	   세션을 이용해 접속시도 -> mh_connect변수를 만들어 접속
	   접속할 주소는 url과 page로 나누어 적어주어야 한다.
	   Url : www.naver.com
	   Page : /이후부분 .htm
	*/
	char error_count = 0;
	// 페이지 정보를 요청한다.
	HINTERNET h_http_file = ::HttpOpenRequest(mh_connect, L"GET", L"/c/entertainment", HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	
	/*
	   Get으로 해당 페이지 요구한다고 요청
	   그다음 page이름 주어야함 .html
	*/
	
	BOOL request_flag = ::HttpSendRequest(h_http_file, NULL, 0, 0, 0);
	// 읽기요청 -> 실제 다운로드 시작

	if (request_flag == TRUE) {
		CString str, str1;
		char *p_html_data = new char[10*1024*1024];
		/* 
			char *p_html_data에 먼저 할당을 해줌
			스택 사이즈는 1메가이므로 배열로 10메가 사이즈 선언하면 문제됨 (에러는 아니지만 안돌아감)
			동적할당 필요-> 동적할당은 2기가까지 사용가능!

		*/
		char buffer[1025];
		DWORD read_byte = 0, buffer_size = 1024, total_size = 0;
		// 최대 1024 크기만큼 웹페이지 정보를 읽는다.
		while (::InternetReadFile(h_http_file, buffer, buffer_size, &read_byte)) {
		// 완전 읽어올때까지 1024byte씩 읽는다.

			buffer[read_byte] = 0;	//읽은 정보 (문자열)의 끝에 NULL(EOL)을 명시한다. 해줘야 각각 읽은 string을 합칠 수 있음!


			if (read_byte > 0 && read_byte < 1025) {
				memcpy(p_html_data + total_size, buffer, read_byte);	//동적할당 한 곳에 계속더해나감
				total_size += read_byte;	
				/* total size: 동적할당에 옮긴 size
				   read_byte: 실제 읽어온 데이터
				   */

			}	
			if (read_byte < 1024) {
				//1024 보다 적은 경우를 읽은 경우, 렉에 의해서 못읽는 경우도 있기 때문에 최대 5번까지 재시도한다.
				// 재시도를 하는 경우 50ms의 지연시간을 부과하여 접속을 기다리도록 한다.
				Sleep(50);
				error_count++;
				if (error_count > 5) break;
				// 5회이상 못읽으면 종료
			}
			else error_count = 0;
		}

		p_html_data[total_size] = 0;  // Null
		//str = p_html_data;
		Convert_UTF8_to_Unicode(p_html_data, &str);


		str.Replace(L"\n", L"\r\n");  // Replace명령어로 \n을 \r\n으로 바꿈
									  // 그래야 Edit Control Box안에서 개행이 가능하다.
		
		SetDlgItemText(IDC_HTML_EDIT, str);
		//해당 페이지 정보를 얻기위해 사용한 핸들을 닫는다.
		::InternetCloseHandle(h_http_file);

		char *p_link_url, *p_pos;

		char temp_name[1024];
		char *p_data = p_html_data;
		while (p_data != NULL) {
			//p_data = strstr(p_data, "cds _MM_CARD");
			p_data = strstr(p_data, "cds_info");
			/* 
				strstr: 문자열 속에서 문자열을 찾음
				읽어온 웹페이지 전체에서 cds _MM_CARD를 찾는다.(영상목록이 있는 위치에는 cds _MM_CARD로 시작함)
				찾으면 null이 아닌 그 글자 맨앞 위치의 주소를 가리킨다. 못찾으면 Null반환
			*/
			if (p_data == NULL) break;
			p_data += 8;					//cds MM_CARD 찾은 이후 +12해주어 그 다음 위치로 이동



			p_data = strstr(p_data, "href=");
			p_data += 6;					//그 다음 href= 을 찾아서 찾은 위치에서 +6 해주어 그 다음 위치로 이동


			p_link_url = new char[1024];	//p_link_url 동적할당함
			strcpy_s(p_link_url, 1024, "https://tv.naver.com"); // 23

			/*
				url(tv.naver.com)을 동적할당한 공간 p_link_url에 복사
			*/
//			p_pos = p_link_url +23;

			p_pos = p_link_url;
			/*
				p_pos에 티비네이버닷컴 복사한 위치+23한 지점에 " 가 나올때 까지 전부 복사함
				 ("부터 " 까지 tvcast.naver.com 이후에 붙여넣음 -> /xxxxx 이런식의 주소가 들어가있으므로..)
			*/
			while (*p_data != '"') {
				*p_pos++ = *p_data++;
			}
			*p_pos = 0;			//마지막 값에 null줌

			char *p_sub_data = strstr(p_data, "<tooltip>");
			// 그 안에서 다시한번 찾아서 <tooltip>을 찾음 (제목이 쓰인 앞의 단어)


			if (p_sub_data != NULL) {
				p_sub_data += 9;		// <tooltip>을 찾은 다음 가리키는 '<'위치에서 +9하면 '>'위치 다음까지 이동함
			
				char *p_end_sub_data = strstr(p_sub_data, "</tooltip>");	//	</tooltip>을 찾는다
				if (p_end_sub_data != NULL) {
					int string_size = p_end_sub_data - p_sub_data;			// </tooltip>을 찾은 위치에서 +9했던 p_sub_data의 위치를 빼면 제목의 길이가 나온다. = string_size
					memcpy(temp_name, p_sub_data, string_size);				// 위에서 1024크기로 할당했던 temp_name에 p_sub_data에서 제목길이만큼 읽어와서 저장
					temp_name[string_size] = 0;								// 마지막에는 null
					Convert_UTF8_to_Unicode(temp_name, &str1);				// web은 UTF8사용, 우리는 유니코드로 쓰기 때문에 안맞아서 한글은 깨져서출력됨 따라서 변환필요
					int index = m_cast_list.InsertString(-1, str1);			// 위 과정을 반복해서 영상 목록들 제목을 전부 리스트로 출력함.
					m_cast_list.SetItemDataPtr(index, p_link_url);			// index에 SetItemDataPtr 로 URl포인터 추가해줌
					//리스트에 적은 내용에 +4바이트의 공간이 남아있으므로 (포인터 크기) 그곳에 URL을 연결시킴
				}

			}

		}
		delete[] p_html_data; 
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CExamNaverTVDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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
	}// 프로그램 종료전 ListBox의 index개수만큼 반복해서 DataPtr 읽어온 후 전부다 동적할당해제

	::InternetCloseHandle(mh_connect);
	::InternetCloseHandle(mh_session);
	//종료될 때 session과 connect종료시킴
}




void CExamNaverTVDlg::OnDblclkCastList()
{// 클래스 마법사에서 명령 - 리스트 - 더블클릭 메시지 발생
	int index = m_cast_list.GetCurSel();
	// 클릭시 파란색으로 커서 체크
	char *p_data = (char *)m_cast_list.GetItemDataPtr(index);
	//해당 index의 데이터 Ptr을 읽어옴 ( 해당 도메인 주소 )
	//void*로 반환하므로 char*로 변환
	//ShellExecuteA(NULL, "Open", p_data, NULL, NULL, SW_SHOW);
	ShellExecuteA(NULL, "Open", "iexplore.exe", p_data, NULL, SW_SHOW);
	//해당 url실행
	//ExcuteA 함수 위의 주석처리처럼 실행하면 디폴트 웹브라우저에 실행됨
	// (이미 인터넷 실행되어있는 경우 그 윈도우에 탭으로 추가됨)
	// iexplore.exe 써서 null값에 url주면 새로 실행시켜서 접속
}

/* 유니코드 변환함수 */
/* 가져올 때 멀티바이트로 가져오는데 유니코드로 바꾸려면 변환을 해야함 */
void Convert_UTF8_to_Unicode(char *ap_src_string, CString *ap_dest_string)
{
	// UTF8 코드로 된 문자열을 유니코드 형식의 문자열로 변경하기 위해 필요한 크기를 얻는다.
	int temp_length = MultiByteToWideChar(CP_UTF8, 0, ap_src_string, -1, NULL, 0);

	if (temp_length > 0) {
		// 변환된 유니코드를 저장할 공간을 할당한다.
		BSTR unicode_str = SysAllocStringLen(NULL, temp_length);
		// 유니코드로 변환한다.
		MultiByteToWideChar(CP_UTF8, 0, ap_src_string, -1, unicode_str, temp_length);
		*ap_dest_string = unicode_str;
		// 유니코드 형식의 문자열을 저장하기 위해 생성했던 메모리를 삭제한다.
		SysFreeString(unicode_str);
	}
	else ap_dest_string->Empty();
}
